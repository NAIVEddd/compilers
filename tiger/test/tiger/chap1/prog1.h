#pragma once 
#include"slp.h"

A_stm prog(void);

int maxargs(A_stm stm);

// interprete prog
Table_ interpStm(A_stm s, Table_ t);
struct IntAndTable {int i; Table_ t;};
struct IntAndTable interpExp(A_exp e, Table_ t);