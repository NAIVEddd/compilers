#pragma once
#include"absyn.h"
#include"../chap5/types.h"
#include"../chap6/translate.h"

typedef void* Tr_exp;

struct expty
{
    Tr_exp exp;
    Ty_ty ty;
};
struct expty expTy(Tr_exp exp, Ty_ty ty);

// do some type check
// F_fragList SEM_transProc(A_exp exp);

struct expty transVar(S_table venv, S_table tenv, Tr_level level, A_var v);
struct expty transExp(S_table venv, S_table tenv, Tr_level level, A_exp a);
void         transDec(S_table venv, S_table tenv, Tr_level level, A_dec d);
       Ty_ty transTy(               S_table tenv, A_ty a);