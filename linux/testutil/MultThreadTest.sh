#!/bin/bash

# Write status to Pipe FD to notify the schedule Success or Error happen
function NotifyPipeWithError()
{
    echo "Error $2" >&$1
}
function NotifyPipeWithSucc()
{
    echo "Success $2" >&$1
}
# Useage : NotifyFD $errno $pipefd [$tobeclosedfd]
function NotifyFD()
{
    if [ $1 != 0 ]
    then 
        NotifyPipeWithError $2 $3
    else
        NotifyPipeWithSucc $2 $3
    fi
}

# Wrapper shell or function to Notify its exit status to pipe fd
# Usage : WrapperXX $pipe Function $FunctionArgs...
function WrapperBash()
{
    {
        trap "echo 'Wrapper exit'" EXIT
        fifoFD=$1
        shift
        $@
        ec=$?
        NotifyFD $ec $fifoFD
    }&
}
function WrapperFunction()
{
    {
        trap "echo 'Wrapper function exit'" EXIT
        fifoFD=$1
        shift
        ec=$($@)
        NotifyFD $ec $fifoFD
    }&
}

function KillAllJobs()
{
    process=$(ps -f | grep -e $1 | grep -v "grep" | awk '{print $2}')
    for pid in $process
    do
        if [ $pid != $1 ]
        then
            KillAllJobs $pid
        fi
    done
    if [ $process'x' != "x" ]
    then
        kill -9 $process
    fi
}

# wait all of the worker thread exit
function ProcessWorkerExitStatus()
{
    pipeFD=$1

    local JobCount=0
    jobs=$(jobs -p)
    for job in $jobs
    do
      JobCount=$(($JobCount+1))
    done

    while [ $JobCount != 0 ]
    do
        read -u $pipeFD stat 
        if [ $? != 0 ]
        then 
            break
        fi
        case $stat in
            Success*)
                JobCount=$(($JobCount-1))
                ;;
            Error*)
                for job in $jobs
                do 
                    KillAllJobs $job
                done
                break
                ;;
            *)
                echo "Unknown msg"
                ;;
        esac
    done

    if [ $JobCount == 0 ]
    then 
        NotifyFD 0 6 $pipeFD
    else
        NotifyFD 1 6 $pipeFD
    fi
}
function SleepTest()
{
    if [ $error != 0 ]
    then
        sleep $1
        echo 5
    else
        sleep $1
        echo 0
    fi
}
function BaseTest()
{
    fd=$1
    shift

    WrapperFunction $fd SleepTest 3
    WrapperFunction $fd SleepTest 4
    WrapperFunction $fd SleepTest 5
    WrapperFunction $fd SleepTest 3
    WrapperFunction $fd SleepTest 4
    WrapperFunction $fd SleepTest 5
            
    ProcessWorkerExitStatus $fd 
}

loopTime=$1
threadnumber=$2

# Open Manager pipe, fd is 6. and start N Thread to execute test
fifo=$(mktemp -u /tmp/fifo.XXXXX)
mkfifo $fifo
exec 6<>"$fifo"
rm -f $fifo
for ((i=0;i<$threadnumber;++i))
do
    NotifyFD 0 6
done

# recv ctrl+c , then kill all worker
trap "NotifyFD 1 6 6" SIGINT

loopcount=1
while [ $loopcount != $(($loopTime+$threadnumber)) ]
do
    read -u 6 status
    case $status in
        Success*)
            loopcount=$(($loopcount+1))
            # wait all task success then exit
            if [ $loopcount -ge $loopTime ]
            then
                continue
            fi
            # close old pipe fd and open new one
            fd=$(echo "$status" | awk '{print $2}')
            if [ $fd'x' != 'x' ]
            then
                exec {fd}>&-
            fi
                Fifo=$(mktemp -u /tmp/fifo.XXXXX)
                mkfifo $Fifo
                exec {MYFD}<>"$Fifo"
                rm -f $Fifo
                fifoFD=${MYFD}
            BaseTest $fifoFD &
            ;;
        Error*)
            fd=$(echo "$status" | awk '{print $2}')
            exec {fd}>&-

            for job in $(jobs -p)
            do
                KillAllJobs $job
            done
            break
            ;;
        *)
            echo "while loopcount recv Unknown error"
            break
            ;;
    esac
done
wait
