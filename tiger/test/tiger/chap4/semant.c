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
        break;
        case A_opExp:
        {
            struct expty left = transExp(venv, tenv, a->u.op.left);
            struct expty right = transExp(venv, tenv, a->u.op.right);
            switch(a->u.op.oper)
            {
                case A_plusOp:
                {
                    return expTy(NULL, Ty_Int());
                }
                break;
                default:
                {
                    return expTy(NULL, Ty_Nil());
                }
            }
        }
        break;
        case A_recordExp:
        {
            E_enventry x = S_look(tenv, S_name(a->u.record.typ));
            if(x && x->kind == E_varEntry)
            {
                return expTy(NULL, actual_ty(x->u.var.ty));
            }
            else
            {
                return expTy(NULL, Ty_Nil());
            }
        }
        break;
        case A_seqExp:
        {
            struct expty res = expTy(NULL, Ty_Nil());
            for(A_expList exps = a->u.seq; exps; exps = exps->tail)
            {
                res = transExp(venv, tenv, exps->head);
            }
            return res;
        }
        break;
        case A_assignExp:
        {
            struct expty var = transVar(venv, tenv, a->u.assign.var);
            struct expty exp = transExp(venv, tenv, a->u.assign.exp);
            if(var.ty == exp.ty)
            {
                return expTy(NULL, var.ty); // should be call actual_ty()?
            }
            else
            {
                return expTy(NULL, Ty_Nil());
            }
        }
        break;
        case A_ifExp:
        {
            struct expty testT = transExp(venv, tenv, a->u.iff.test);
            if(testT.ty->kind != Ty_int)
            {
                return expTy(NULL, Ty_Nil());
            }
            struct expty thenT = transExp(venv, tenv, a->u.iff.then);
            struct expty elseT = transExp(venv, tenv, a->u.iff.elsee);
            if(thenT.ty->kind == elseT.ty->kind)    // actual_ty(then) == actual_ty(else)
            {
                thenT.exp = NULL;
                return thenT;
            }
            else
            {
                return expTy(NULL, Ty_Nil());
            }
        }
        break;
        case A_whileExp:
        {
            struct expty testT = transExp(venv, tenv, a->u.whilee.test);
            if(testT.ty->kind != Ty_int)
            {
                return expTy(NULL, Ty_Nil());
            }
            return transExp(venv, tenv, a->u.whilee.body);
        }
        break;
        case A_forExp:
        {
            
        }
        break;

        // {enum {
	    //    , , , , ,
	    //    , , A_breakExp, A_letExp, A_arrayExp} kind;
    }
}
void transDec(S_table venv, S_table tenv, A_dec d);
Ty_ty transTy(S_table tenv, A_ty a);