#include "../../testCase.h"
#include "table.h"
#include "semant.h"
#include "env.h"
#include "parse.h"
#include "../chap7/printtree.h"
#include <stdio.h>
A_exp absyn_root;

int main()
{
    {
        Tr_level level = Tr_Outermost();
        S_table venv = E_base_venv();
        S_table tenv = E_base_tenv();
        S_enter(tenv, S_Symbol("void"), Ty_Void());
        S_enter(tenv, S_Symbol("int"), Ty_Int());
        S_enter(tenv, S_Symbol("string"), Ty_String());
        S_enter(tenv, S_Symbol("nil"), Ty_Nil());
        S_enter(venv, S_Symbol("print"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_String(), NULL), Ty_Void()));
        
        struct expty ety = transExp(venv, tenv, level, MakeQueensTig());
        printStmList(stdout, T_StmList(T_Exp(ety.exp), NULL));
    }

    return 0;
}