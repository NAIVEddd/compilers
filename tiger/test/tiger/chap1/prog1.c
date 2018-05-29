#include<stdio.h>
#include"prog1.h"
#include "util.h"
#include "slp.h"

A_stm prog(void)
{

    return A_CompoundStm(A_AssignStm("a",
                                     A_OpExp(A_NumExp(5), A_plus, A_NumExp(3))),
                         A_CompoundStm(A_AssignStm("b",
                                                   A_EseqExp(A_PrintStm(A_PairExpList(A_IdExp("a"),
                                                                                      A_LastExpList(A_OpExp(A_IdExp("a"), A_minus,
                                                                                                            A_NumExp(1))))),
                                                             A_OpExp(A_NumExp(10), A_times, A_IdExp("a")))),
                                       A_PrintStm(A_LastExpList(A_IdExp("b")))));
}

int maxargs(A_stm stm)
{
    int res = 0;
    switch (stm->kind)
    {
    case A_compoundStm:
    {
        res = maxargs(stm->u.compound.stm1) + maxargs(stm->u.compound.stm2);
    }
    break;
    case A_assignStm:
    {
        int k = stm->u.assign.exp->kind;
        if(k == A_eseqExp)
        {
            res = maxargs(stm->u.assign.exp->u.eseq.stm);
        }
    }
    break;
    case A_printStm:
    {
        A_expList tmp = stm->u.print.exps;
        for(res = 1; tmp->kind != A_lastExpList; tmp = tmp->u.pair.tail)
        {
            res += 1;
        }
    }
    break;
    default:
        break;
    }
    return res;
}

Table_ interpStm(A_stm s, Table_ t)
{
    switch(s->kind)
    {
        case A_compoundStm:
        {
            A_stm stm1 = s->u.compound.stm1, stm2 = s->u.compound.stm2;
            t = interpStm(stm1, t);
            t = interpStm(stm2, t);
        }
        break;
        case A_assignStm:
        {
            struct IntAndTable iat = interpExp(s->u.assign.exp, t);
            t = update(iat.t, s->u.assign.id, iat.i);
        }
        break;
        case A_printStm:
        {
            A_expList exps = s->u.print.exps;
            while(1)
            {
                if(exps->kind == A_pairExpList)
                {
                    struct IntAndTable iat = interpExp(exps->u.pair.head, t);
                    printf("%d ", iat.i);
                    exps = exps->u.pair.tail;
                }
                else
                {
                    struct IntAndTable iat = interpExp(exps->u.last, t);
                    printf("%d\n", iat.i);
                    break;
                }
            }
        }
        break;
        default:
        printf("Unknown stm\n");
        break;
    }
    return t;
}

struct IntAndTable interpExp(A_exp e, Table_ t)
{
    struct IntAndTable iat;
    switch(e->kind)
    {
        case A_idExp:
        {
            iat.i = lookup(t, e->u.id);
            iat.t = t;
        }
        break;
        case A_numExp:
        {
            iat.i = e->u.num;
            iat.t = t;
        }
        break;
        case A_opExp:
        {
            int tmp = 0;
            struct IntAndTable iat1, iat2;
            iat1 = interpExp(e->u.op.left, t);
            iat2 = interpExp(e->u.op.right, iat1.t);
            switch(e->u.op.oper)
            {
                case A_plus:
                tmp = iat1.i + iat2.i;
                break;
                case A_minus:
                tmp = iat1.i - iat2.i;
                break;
                case A_times:
                tmp = iat1.i * iat2.i;
                break;
                case A_div:
                tmp = iat1.i / iat2.i;
                break;
                default:
                printf("Unknown oper.\n");
                break;
            }
            iat.i = tmp;
            iat.t = iat2.t;
        }
        break;
        case A_eseqExp:
        {
            t = interpStm(e->u.eseq.stm, t);
            iat = interpExp(e->u.eseq.exp, t);
        }
        break;
        default:
        printf("Unknown exp.\n");
        break;
    }
    return iat;
}