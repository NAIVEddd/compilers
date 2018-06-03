#include<stdio.h>
#include "semant.h"
#include "env.h"
#include "errormsg.h"

struct expty expTy(Tr_exp exp, Ty_ty ty)
{
    struct expty e;
    e.exp = exp;
    e.ty = ty;
    return e;
}

struct expty transVar(S_table venv, S_table tenv, A_var v)
{
    switch (v->kind)
    {
    case A_simpleVar:
    {
        E_enventry x = S_look(venv, v->u.simple);
        if (x && x->kind == E_varEntry)
        {
            return expTy(NULL, actual_ty(x->u.var.ty));
        }
        else
        {
            EM_error(v->pos, "undefined variable %s", S_name(v->u.simple));
            return expTy(NULL, Ty_Int());
        }
    }
    break;
    case A_fieldVar:
    {
        if (v->u.field.var->kind != A_simpleVar)
        {
            EM_error(v->pos, "filed var %s not a valid record type.\n", S_name(v->u.field.sym));
            return expTy(NULL, Ty_Int());
        }
        E_enventry x = S_look(venv, v->u.field.var->u.simple);
        if (x && x->kind == E_varEntry && x->u.var.ty->kind == Ty_record)
        {
            for (Ty_fieldList field = x->u.var.ty->u.record; field != NULL; field = field->tail)
            {
                if (field->head->name == v->u.field.sym)
                {
                    return expTy(NULL, actual_ty(field->head->ty));
                }
                else
                {
                    EM_error(v->pos, "undefine var's field %s", S_name(v->u.field.sym));
                    return expTy(NULL, Ty_Int());
                }
            }
        }
        else
        {
            EM_error(v->pos, "undefine var's field1 %s", S_name(v->u.field.sym));
            return expTy(NULL, Ty_Int());
        }
    }
    break;
    case A_subscriptVar:
    {
        if(v->u.subscript.var->kind != A_simpleVar)
        {
            EM_error(v->pos, "Subscript var type is not simpleVar: %s", S_name(v->u.subscript.var->u.simple));
            return expTy(NULL, Ty_Int());
        }
        E_enventry x = S_look(venv, v->u.subscript.var->u.simple);
        if(x && x->kind == E_varEntry)
        {
            return expTy(NULL, x->u.var.ty);
        }
        else
        {
            EM_error(v->pos, "undefined varialbe %s", S_name(v->u.subscript.var->u.simple));
            return expTy(NULL, Ty_Int());
        }
    }
    break;
    default:
        printf("semant transVar default patch.\n");
        break;
    }
}
struct expty transExp(S_table venv, S_table tenv, A_exp a)
{
    switch(a->kind)
    {
        case A_varExp:
        {
            return transVar(venv, tenv, a->u.var);
        }
        break;
        case A_nilExp:
        {
            return expTy(NULL, Ty_Nil());
        }
        break;
        case A_intExp:
        {
            return expTy(NULL, Ty_Int());
        }
        break;
        case A_stringExp:
        {
            return expTy(NULL, Ty_String());
        }
        break;
        case A_callExp:
        {
            E_enventry x = S_look(venv, a->u.call.func);
            if(x && x->kind == E_funEntry)
            {
                Ty_tyList paramT = x->u.fun.formals;
                A_expList paramV = a->u.call.args;
                for(; paramT && paramV; paramT = paramT->tail, paramV = paramV->tail)
                {
                    struct expty vT = transExp(venv, tenv, paramV->head);
                    if(paramT->head->kind != vT.ty->kind)
                        break;
                }
                if(paramT || paramV)
                {
                    EM_error(a->pos, "func %s args not matched.", a->u.call.func);
                    return expTy(NULL, x->u.fun.result);
                }
                return expTy(NULL, x->u.fun.result);
            }
        }

        // {enum {A_callExp,
	    //    A_opExp, A_recordExp, A_seqExp, A_assignExp, A_ifExp,
	    //    A_whileExp, A_forExp, A_breakExp, A_letExp, A_arrayExp} kind;
    }
}
void transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, A_ty a);