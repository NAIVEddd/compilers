#include<stdio.h>
#include"prog1.h"
#include"slp.h"
#include"util.h"

int main()
{
    // printf("%d\n", maxargs(prog()));
    Table_ t = Table("", 0, NULL);
    interpStm(prog(), t);
    return 0;
}