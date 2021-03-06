#include "../../testCase.h"
#include "table.h"
#include "semant.h"
#include "env.h"
#include "parse.h"
#include "../chap7/printtree.h"
#include "../chap8/canon.h"
#include "../chap9/codegen.h"
#include "../chap6/frame.h"
#include <stdio.h>
A_exp absyn_root;

int main()
{
    AS_instrList instrList;
    {
        Tr_level level = Tr_Outermost();
        S_table venv = E_base_venv();
        S_table tenv = E_base_tenv();
        S_enter(tenv, S_Symbol("void"), Ty_Void());
        S_enter(tenv, S_Symbol("int"), Ty_Int());
        S_enter(tenv, S_Symbol("string"), Ty_String());
        S_enter(tenv, S_Symbol("nil"), Ty_Nil());
        S_enter(venv, S_Symbol("print"), E_FunEntry(level, Temp_newlabel(), Ty_TyList(Ty_String(), NULL), Ty_Void()));
        
        F_tempMap = Temp_empty();

        struct expty ety = transExp(venv, tenv, level, MakeQueensTig());
        T_stmList stmList = C_linearize(T_Exp(ety.exp));
        struct C_block block = C_basicBlocks(stmList);
        stmList = C_traceSchedule(block);
        printStmList(stdout, stmList);

        F_frame frame = F_NewFrame(Temp_newlabel(), NULL);
        instrList = F_codegen(frame, stmList);
        Temp_dumpMap(stdout, F_tempMap);
        fprintf(stdout, "BEGIN %s\n", Temp_labelstring(F_Name(frame)));
        AS_printInstrList(stdout, instrList, Temp_layerMap(F_tempMap,Temp_name()));
    }

    return 0;
}