#include "../../testCase.h"
#include "table.h"
#include "semant.h"
#include "env.h"

int main()
{
    A_fundec func = funcF();
    {
        A_pos nilPos = 0;
        S_symbol smb_a = S_Symbol("a");
        S_symbol smb_j = S_Symbol("j");
        S_symbol smb_var = S_Symbol("var");
        A_dec dec_j = A_VarDec(nilPos, smb_j, smb_var, A_VarExp(nilPos, A_SimpleVar(nilPos, smb_a)));
        S_table tenv = E_base_tenv();
        S_enter(tenv, dec_j->u.type->head->name, dec_j->u.type->head->ty); // actual_ty
        S_table venv = E_base_venv();
        // S_enter(venv, dec_j->u.var.var, )
        struct expty ety = transVar(tenv, tenv, A_SimpleVar(nilPos, smb_a));
    }
    return 0;
}