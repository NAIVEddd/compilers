#include <stdio.h>
#include "absyn.h"
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

struct expty transVar(S_table venv, S_table tenv, Tr_level level, A_var v)
{
    switch (v->kind)
    {
    case A_simpleVar:
    {
        E_enventry x = S_look(venv, v->u.simple);
        if (x && x->kind == E_varEntry)
        {
            return expTy(Tr_simpleVar(x->u.var.access, level), actual_ty(x->u.var.ty));
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
            }
        }
        EM_error(v->pos, "undefine var's field %s", S_name(v->u.field.sym));
        return expTy(NULL, Ty_Int());
    }
    break;
    case A_subscriptVar:
    {
        if (v->u.subscript.var->kind != A_simpleVar)
        {
            EM_error(v->pos, "Subscript var type is not simpleVar: %s", S_name(v->u.subscript.var->u.simple));
            return expTy(NULL, Ty_Int());
        }
        E_enventry x = S_look(venv, v->u.subscript.var->u.simple);
        if (x && x->kind == E_varEntry)
        {
            struct expty i = transExp(venv, tenv, level, v->u.subscript.exp);
            return expTy(Tr_subscriptVar(x->u.var.access, level, i.exp), actual_ty(x->u.var.ty)->u.array);
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
struct expty transExp(S_table venv, S_table tenv, Tr_level level, A_exp a)
{
    switch (a->kind)
    {
    case A_varExp:
    {
        return transVar(venv, tenv, level, a->u.var);
    }
    break;
    case A_nilExp:
    {
        return expTy(T_Const(0), Ty_Nil()); // todo: should't use T_Const(0).
    }
    break;
    case A_intExp:
    {
        return expTy(Tr_int(a->u.intt), Ty_Int());
    }
    break;
    case A_stringExp:
    {
        return expTy(Tr_String(a->u.stringg), Ty_String());
    }
    break;
    case A_callExp:
    {
        E_enventry x = S_look(venv, a->u.call.func);
        if (x && x->kind == E_funEntry)
        {
            Ty_tyList paramT = x->u.fun.formals;
            A_expList paramV = a->u.call.args;
            Tr_level func_level = Tr_NewLevel(level, Temp_newlabel(), NULL);
            Tr_access fp = Tr_Formals(func_level)->head;
            T_exp static_link = Tr_StaticLink(fp, func_level);
            T_expList args = T_ExpList(static_link, NULL);
            T_expList *tail = &(args->tail);
            for (; paramT && paramV; paramT = paramT->tail, paramV = paramV->tail)
            {
                struct expty vT = transExp(venv, tenv, func_level, paramV->head);
                *tail = T_ExpList(vT.exp, NULL);
                tail = &((*tail)->tail);
                if (paramT->head->kind != vT.ty->kind)
                    break;
            }
            if (paramT || paramV)
            {
                EM_error(a->pos, "func %s args not matched.", a->u.call.func);
                return expTy(NULL, x->u.fun.result);
            }
            return expTy(T_Call(T_Name(x->u.fun.label), args), x->u.fun.result);
        }
    }
    break;
    case A_opExp:
    {
        struct expty left = transExp(venv, tenv, level, a->u.op.left);
        struct expty right = transExp(venv, tenv, level, a->u.op.right);
        switch (a->u.op.oper)
        {
        case A_plusOp:
        {
            return expTy(T_Binop(T_plus, left.exp, right.exp), Ty_Int());
        }
        break;
        case A_minusOp:
        {
            return expTy(T_Binop(T_minus, left.exp, right.exp), Ty_Int());
        }
        break;
        case A_timesOp:
        {
            return expTy(T_Binop(T_mul, left.exp, right.exp), Ty_Int());
        }
        break;
        case A_divideOp:
        {
            return expTy(T_Binop(T_div, left.exp, right.exp), Ty_Int());
        }
        break;
        default:
        {
            Temp_label t = Temp_newlabel(), f = Temp_newlabel();
            T_exp t_exp = T_Const(1), f_exp = T_Const(0);
            T_relOp op;
            switch (a->u.op.oper)
            {
            case A_eqOp:
            {
                op = T_eq;
            }
            break;
            case A_neqOp:
            {
                op = T_ne;
            }
            break;
            case A_ltOp:
            {
                op = T_lt;
            }
            break;
            case A_leOp:
            {
                op = T_le;
            }
            break;
            case A_gtOp:
            {
                op = T_gt;
            }
            break;
            case A_geOp:
            {
                op = T_ge;
            }
            break;
            default:
                assert(0);
                break;
            }

            T_stm t_stm = T_Seq(T_Label(t), T_Exp(t_exp)), f_stm = T_Seq(T_Label(f), T_Exp(f_exp));
            return expTy(T_Eseq(T_Seq(T_Cjump(op, left.exp, right.exp, t, f), T_Seq(t_stm, f_stm)), T_Const(0)), Ty_Int());
        }
        }
    }
    break;
    case A_recordExp:
    {
        E_enventry x = S_look(tenv, a->u.record.typ);
        if (x && x->kind == E_varEntry)
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
        T_stm stm = NULL, *stmTail = &stm;
        for (A_expList exps = a->u.seq; exps; exps = exps->tail)
        {
            res = transExp(venv, tenv, level, exps->head);
            if (*stmTail == NULL)
                *stmTail = T_Exp(res.exp);
            else
                *stmTail = T_Seq(*stmTail, T_Exp(res.exp));
        }
        res.exp = T_Eseq(stm, T_Const(0));
        return res;
    }
    break;
    case A_assignExp:
    {
        struct expty var = transVar(venv, tenv, level, a->u.assign.var);
        struct expty exp = transExp(venv, tenv, level, a->u.assign.exp);
        if (var.ty == exp.ty)
        {
            return expTy(T_Eseq(T_Move(var.exp, exp.exp), T_Const(0)), var.ty); // should be call actual_ty()?
        }
        else
        {
            assert(0);
            return expTy(NULL, Ty_Nil());
        }
    }
    break;
    case A_ifExp:
    {
        // test is binOpExp
        // if test true, goto true label, then exec then body and goto end label
        // if test false, goto false label, then exec else body and goto end label
        // ifExp don't return a value.

        T_relOp op;
        if (a->u.iff.test->kind != A_opExp)
        {
            assert(0);
        }

        switch (a->u.iff.test->u.op.oper)
        {
            case A_eqOp:
            {
                op = T_eq;
            }
            break;
            case A_neqOp:
            {
                op = T_ne;
            }
            break;
            case A_ltOp:
            {
                op = T_lt;
            }
            break;
            case A_leOp:
            {
                op = T_le;
            }
            break;
            case A_gtOp:
            {
                op = T_gt;
            }
            break;
            case A_geOp:
            {
                op = T_ge;
            }
            break;
            default:
                assert(0);
                break;
        }
        struct expty left = transExp(venv, tenv, level, a->u.iff.test->u.op.left);
        struct expty right = transExp(venv, tenv, level, a->u.iff.test->u.op.right);

        Temp_label t = Temp_newlabel();
        Temp_label f = Temp_newlabel();
        Temp_label if_end = Temp_newlabel();
        T_stm test = T_Cjump(op, left.exp, right.exp, t, f);
        struct expty if_then = transExp(venv, tenv, level, a->u.iff.then);
        T_stm then = T_Seq(T_Label(t), 
                        T_Seq(T_Exp(if_then.exp), 
                            T_Seq(T_Jump(T_Name(if_end), Temp_LabelList(if_end, NULL)), 
                                    T_Label(if_end))));
        T_stm elsee = NULL;
        if (a->u.iff.elsee == NULL)
        {
            elsee = T_Seq(T_Label(f), 
                        T_Seq(T_Exp(T_Const(0)),
                            T_Seq(T_Jump(T_Name(if_end), Temp_LabelList(if_end, NULL)),
                                T_Label(if_end))));
        }
        else
        {
            struct expty if_else = transExp(venv, tenv, level, a->u.iff.elsee);
            // elsee = T_Seq(T_Label(f), T_Jump(if_else.exp, Temp_LabelList(if_end, NULL)));
            elsee = T_Seq(T_Label(f),
                        T_Seq(T_Exp(if_else.exp),
                            T_Seq(T_Jump(T_Name(if_end), Temp_LabelList(if_end, NULL)),
                                T_Label(if_end))));
        }
        T_exp result = T_Eseq(T_Seq(test, T_Seq(then, elsee)), T_Const(0));
        return expTy(result, if_then.ty);
    }
    break;
    case A_whileExp:
    {
        struct expty testT = transExp(venv, tenv, level, a->u.whilee.test);
        if (testT.ty->kind != Ty_int)
        {
            printf("while test exp must been int exp.\n");
            return expTy(NULL, Ty_Nil());
        }
        return transExp(venv, tenv, level, a->u.whilee.body);
    }
    break;
    case A_forExp:
    {
        S_symbol limit = Temp_newlabel();
        A_decList var_defs = A_DecList(A_VarDec(a->pos, a->u.forr.var, NULL, a->u.forr.lo), A_DecList(A_VarDec(a->pos, limit, NULL, a->u.forr.hi), NULL));
        A_exp let_exp = A_LetExp(a->pos, var_defs,
                                 A_WhileExp(a->pos, A_OpExp(a->pos, A_leOp, A_VarExp(a->pos, A_SimpleVar(a->pos, a->u.forr.var)), A_VarExp(a->pos, A_SimpleVar(a->pos, limit))),
                                            A_SeqExp(a->pos, A_ExpList(a->u.forr.body, A_ExpList(A_OpExp(a->pos, A_plusOp, A_VarExp(a->pos, A_SimpleVar(a->pos, a->u.forr.var)), A_IntExp(a->pos, 1)), NULL)))));

        // S_beginScope(venv);
        // Tr_level forLevel = Tr_NewLevel(level, Temp_newlabel(), NULL);
        // A_dec dec_var = ;
        // transDec(venv, tenv, forLevel, dec_var);

        // struct expty loT = transExp(venv, tenv, forLevel, a->u.forr.lo);
        // struct expty hiT = transExp(venv, tenv, forLevel, a->u.forr.hi);
        // struct expty bodyT = transExp(venv, tenv, forLevel, a->u.forr.body);
        // S_endScope(venv);
        return transExp(venv, tenv, level, let_exp);
    }
    break;
    case A_breakExp:
    {
    }
    break;
    case A_letExp:
    {
        struct expty exp;
        A_decList d;
        S_beginScope(venv);
        S_beginScope(tenv);
        Tr_level letLevel = Tr_NewLevel(level, Temp_newlabel(), NULL);
        T_stm decList = NULL;
        T_stm *listTail = &decList;
        for (d = a->u.let.decs; d; d = d->tail)
        {
            Tr_exp dec = transDec(venv, tenv, letLevel, d->head);
            if (*listTail != NULL)
                *listTail = T_Seq(*listTail, T_Exp(dec));
            else
                *listTail = T_Exp(dec);
        }
        exp = transExp(venv, tenv, letLevel, a->u.let.body);
        exp.exp = T_Eseq(decList, exp.exp);
        S_endScope(tenv);
        S_endScope(venv);
        return exp;
    }
    break;
    case A_arrayExp:
    {
        Ty_ty varT = S_look(tenv, a->u.array.typ);
        struct expty expSz = transExp(venv, tenv, level, a->u.array.size);
        struct expty expInit = transExp(venv, tenv, level, a->u.array.init);
        return expTy(Tr_ArrayInit(expSz.exp, expInit.exp), varT);
    }
    break;
    default:
    {
        assert(0); // should't run to this line.
        return expTy(NULL, Ty_Nil());
    }
    break;
    }
    assert(0);
}

Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params)
{
    Ty_tyList tmp = NULL;
    for (A_fieldList l = params; l; l = l->tail)
    {
        tmp = Ty_TyList(S_look(tenv, l->head->typ), tmp);
    }
    Ty_tyList res = NULL;
    for (; tmp; tmp = tmp->tail)
    {
        res = Ty_TyList(tmp->head, res);
    }
    return res;
}

Tr_exp transDec(S_table venv, S_table tenv, Tr_level level, A_dec d)
{
    switch (d->kind)
    {
    case A_functionDec:
    {
        for (A_fundecList l = d->u.function; l; l = l->tail)
        {
            A_fundec f = l->head;
            Ty_ty resultTy = S_look(tenv, f->result);
            Ty_tyList formalTys = makeFormalTyList(tenv, f->params);
            S_enter(venv, f->name, E_FunEntry(level, Temp_newlabel(), formalTys, resultTy));
        }

        for (A_fundecList fl = d->u.function; fl; fl = fl->tail)
        {
            A_fundec f = fl->head;
            Ty_tyList formalTys = ((E_enventry)S_look(venv, f->name))->u.fun.formals;
            S_beginScope(venv);
            {
                A_fieldList l;
                Ty_tyList t;
                for (l = f->params, t = formalTys; l; l = l->tail, t = t->tail)
                {
                    Tr_access varLocate = Tr_AllocLocal(level, TRUE);
                    S_enter(venv, l->head->name, E_VarEntry(varLocate, t->head));
                }
            }
            transExp(venv, tenv, level, f->body);
            S_endScope(venv);
        }

        // temp impl.
        return T_Const(0);
        assert(0);
    }
    break;
    case A_varDec:
    {
        if (!d->u.var.init && !d->u.var.typ)
        {
            assert(0);
        }

        Tr_access mem = Tr_AllocLocal(level, TRUE);
        struct expty res;
        if (d->u.var.init)
        {
            struct expty e = transExp(venv, tenv, level, d->u.var.init);
            res.exp = T_Eseq(T_Move(Tr_simpleVar(mem, level), e.exp), T_Const(0));
            S_enter(venv, d->u.var.var, E_VarEntry(mem, e.ty));
        }
        return res.exp;
    }
    break;
    case A_typeDec:
    {
        A_nametyList types;
        for (types = d->u.type; types; types = types->tail)
        {
            S_enter(tenv, types->head->name, Ty_Name(types->head->name, NULL));
        }
        for (types = d->u.type; types; types = types->tail)
        {
            Ty_ty name = S_look(tenv, types->head->name);
            name->u.name.ty = transTy(tenv, d->u.type->head->ty);
        }

        // temp impl
        return T_Const(0);
        assert(0);
    }
    break;
    default:
    {
        printf("transDec fail.\n");
    }
    break;
    }
}
Ty_ty transTy(S_table tenv, A_ty a)
{
    switch (a->kind)
    {
    case A_nameTy:
    {
        Ty_ty ty = S_look(tenv, a->u.name);
        return ty->u.name.ty;
    }
    break;
    case A_arrayTy:
    {
        return Ty_Array(S_look(tenv, a->u.array));
    }
    break;
    case A_recordTy:
    {
        Ty_fieldList tmp = NULL;
        for (A_fieldList l = a->u.record; l; l = l->tail)
        {
            Ty_field head = Ty_Field(l->head->name, S_look(tenv, l->head->typ));
            tmp = Ty_FieldList(head, tmp);
        }
        Ty_fieldList res = NULL;
        for (; tmp; tmp = tmp->tail)
        {
            res = Ty_FieldList(tmp->head, res);
        }
        return Ty_Record(res);
    }
    break;
    default:
    {
        printf("transTy fail.\n");
        return Ty_Nil();
    }
    break;
    }
}
