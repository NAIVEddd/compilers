#include"codegen.h"
#include"../chap4/util.h"
#include"../chap4/absyn.h"

static AS_instrList instrList = NULL, instrListLast = NULL;

static void emit(AS_instr instr);
static void munchStm(T_stm stm);
static Temp_temp munchExp(T_exp expr);
static Temp_tempList munchArgs(uint32_t n, T_expList eList, F_accessList formals);

static F_frame CODEGEN_frame = NULL;    // used in munchArgs


static void emit(AS_instr instr)
{
    if(!instrList) instrList = instrListLast = AS_InstrList(instr, NULL);
    else    instrListLast = instrListLast->tail = AS_InstrList(instr, NULL);
}

static void munchStm(T_stm stm)
{
    switch(stm->kind)
    {
        case T_SEQ:
        {
            munchStm(stm->u.SEQ.left);
            munchStm(stm->u.SEQ.right);
        }
        break;
        case T_LABEL:
        {
            emit(AS_Label(String_format("%s:\n", Temp_labelstring(stm->u.LABEL)),
                    stm->u.LABEL));
        }
        break;
        case T_JUMP:
        {
            Temp_temp r = munchExp(stm->u.JUMP.exp);
            emit(AS_Oper(String_format("jmp `d0\n"), Temp_LabelList(r, NULL),NULL,
                            AS_Targets(stm->u.JUMP.jumps)));
        }
        break;
        case T_CJUMP:
        {
            Temp_temp left = munchExp(stm->u.CJUMP.left),
                        right = munchExp(stm->u.CJUMP.right);
            emit(AS_Oper(String_format("cmp `s0, s`\n"), NULL, Temp_TempList(left, Temp_TempList(right, NULL)),
                    NULL));
            
            char * instr = NULL;
            switch(stm->u.CJUMP.op)
            {
                case T_eq:
                    instr = "je"; break;
                case T_ne:
                    instr = "jne"; break;
                case T_lt:
                    instr = "jl"; break;
                case T_gt:
                    instr = "jg"; break;
                case T_le:
                    instr = "jle"; break;
                case T_ge:
                    instr = "jge"; break;
                default: assert(0);
            }
            emit(AS_Oper(String_format("%s `j0\n", instr), NULL, NULL, AS_Targets(Temp_LabelList(stm->u.CJUMP.true, NULL))));

        }
        break;
        case T_MOVE:
        {
            T_exp dst = stm->u.MOVE.dst, src = stm->u.MOVE.src;
            if(dst->kind == T_MEM)
            {
                if(dst->u.MEM->kind == T_BINOP
                    && dst->u.MEM->u.BINOP.op == T_plus
                    && dst->u.MEM->u.BINOP.right->kind == T_CONST)
                    {
                        T_exp e1 = dst->u.MEM->u.BINOP.left, e2 = src;
                        /*MOVE(MEM(BINOP(PLUS, e1, CONST(i))), e2)*/
                        Temp_temp t1 = munchExp(e1), t2 = munchExp(e2); 
                        emit(AS_Move("mov `d0, `s0\n", Temp_TempList(t1, NULL), Temp_TempList(t2, NULL)));
                    }
                else if(dst->u.MEM->kind == T_BINOP
                    && dst->u.MEM->u.BINOP.op == T_plus
                    && dst->u.MEM->u.BINOP.left->kind == T_Const)
                    {
                        T_exp e1 = dst->u.MEM->u.BINOP.right, e2 = src;
                        /*MOVE(MEM(BINOP(PLUS, CONST(i), e1)), e2)*/
                        Temp_temp t1 = munchExp(e1), t2 = munchExp(e2);
                        emit(AS_Move("mov `d0, `so\n", Temp_TempList(t1, NULL), Temp_TempList(t2, NULL)));
                    }
                else if(src->kind == T_MEM)
                    {
                        T_exp e1 = dst->u.MEM, e2 = src = src->u.MEM;
                        /*MOVE(MEM(e1), MEM(e2))*/
                        Temp_temp t1 = munchExp(e1), t2 = munchExp(e2);
                        emit(AS_Move("mov `m0, `m1\n", Temp_TempList(t1, NULL), Temp_TempList(t2, NULL)));
                    }
                else
                {
                    T_exp e1 = dst->u.MEM, e2 = src;
                    /*MOVE(MEM(e1), e2)*/
                    Temp_temp t1 = munchExp(e1), t2 = munchExp(e2);
                    emit(AS_Move("mov `m0, `s0\n", Temp_TempList(t1, NULL), Temp_TempList(t2, NULL)));
                }
            }
            else if(dst->kind == T_TEMP)
            {
                /*MOVE(TEMP i, e2)*/
                emit(AS_Move("mov `d0, `s0\n", Temp_LabelList(munchExp(dst), NULL), Temp_LabelList(munchExp(src), NULL)));
            }
            else
                assert(0);
        }
        break;
        case T_EXP:
        {
            munchExp(stm->u.EXP);
        }
        break;
        default: assert(0);
    }
}

static Temp_temp munchExp(T_exp expr)
{
    switch(expr->kind)
    {
        case T_BINOP:
        {
            T_exp left = expr->u.BINOP.left, right = expr->u.BINOP.right;
            Temp_temp r = Temp_newtemp();
            char const * opStr = NULL, *sign = NULL;
            switch(expr->u.BINOP.op)
            {
                case A_plusOp:
                {
                    if(left->kind == T_CONST && left->u.CONST == 0)
                    {
                        return munchExp(right);
                    }
                    else if(right->kind == T_CONST && right->u.CONST == 0)
                    {
                        return munchExp(left);
                    }

                    if(left->kind == T_CONST)
                    {
                        emit(AS_Oper(String_format("add `d0, `s0(%s%d)\n", "+", left->u.CONST),
                                        Temp_LabelList(r, NULL),  Temp_LabelList(munchExp(right), NULL), NULL));
                        return r;
                    }
                    else if(right->kind == T_CONST)
                    {
                        emit(AS_Oper(String_format("add `d0, `s0(%s%d)\n", "+", right->u.CONST),
                                        Temp_LabelList(r, NULL), Temp_LabelList(munchExp(left), NULL), NULL));
                        return r;
                    }
                    else
                    {
                        emit(AS_Oper(String_format("add `d0, `s0(%s)`s1\n", "+"),
                                        Temp_LabelList(r, NULL), Temp_LabelList(munchExp(left), Temp_LabelList(munchExp(right), NULL)), NULL));
                        return r;
                    }
                }
                break;
                case A_minusOp:
                {
                    opStr = "sub";
                    sign = "-";
                }
                break;
                case A_timesOp:
                {
                    opStr = "mul";
                    sign = "*";
                }
                break;
                case A_divideOp:
                {
                    opStr = "div";
                    sign = "/";
                }
                break;
                default: assert(0);
            }
            if(left->kind == T_CONST)
            {
                emit(AS_Oper(String_format("%s `d1, `s0(%s%d)\n", opStr, sign, left->u.CONST),
                                Temp_LabelList(r, NULL), Temp_LabelList(munchExp(right), NULL), NULL));
                return r;
            }
            else if(right->kind == T_CONST)
            {
                emit(AS_Oper(String_format("%s `d1, `s0(%s%d)\n", opStr, sign, right->u.CONST),
                                Temp_LabelList(r, NULL), Temp_LabelList(munchExp(left), NULL), NULL));
                return r;
            }
            else
            {
                emit(AS_Oper(String_format("%s `d1, `s0(%s)s1\n", opStr, sign),
                                Temp_LabelList(r, NULL), Temp_LabelList(munchExp(left), Temp_LabelList(munchExp(right), NULL)), NULL));
                return r;
            }
        }
        break;
        case T_MEM:
        {
            T_exp mem = expr->u.MEM;
            Temp_temp r = Temp_newtemp();
            if(mem->kind == T_BINOP && mem->u.BINOP.op == T_plus)
            {
                T_exp left = mem->u.BINOP.left, right = mem->u.BINOP.right;
                if(left->kind == T_CONST)
                {
                    /*MEM(BINOP(PLUS, CONST(i), e2))*/
                    emit(AS_Move(String_format("mov `d0, `s0 + %d\n", left->u.CONST),
                            Temp_TempList(r, NULL), Temp_TempList(munchExp(right), NULL)));
                    return r;
                }
                else if(right->kind == T_CONST)
                {
                    /*MEM(BINOP(PLUS, e1, CONST(i)))*/
                    emit(AS_Move(String_format("mov `d0, `s0 + %d\n", right->u.CONST),
                            Temp_TempList(r, NULL), Temp_TempList(munchExp(left), NULL)));
                    return r;
                }
                else
                {
                    assert(0);
                }
            }
            else if(mem->kind == T_CONST)
            {
                assert(0);
                /*MEM(CONST(i))*/
                emit(AS_Move(String_format("mov `d0, [%d]\n", mem->u.CONST),
                        Temp_TempList(r, NULL), NULL));
                return r;
            }
            else
            {
                /*MEM(e1)*/
                emit(AS_Move(String_format("mov `d0, [`s0]\n"),
                        Temp_TempList(r, NULL), Temp_TempList(munchExp(mem->u.MEM), NULL)));
                return r;
            }
        }
        break;
        case T_TEMP:
        {
            return expr->u.TEMP;
        }
        break;
        case T_ESEQ:
        {
            munchStm(expr->u.ESEQ.stm);
            return munchExp(expr->u.ESEQ.exp);
        }
        break;
        case T_NAME:
        {
            Temp_temp r = Temp_newtemp();
            Temp_enter(F_tempMap, r, Temp_labelstring(expr->u.NAME));
            return r;
        }
        break;
        case T_CONST:
        {
            Temp_temp r = Temp_newtemp();
            emit(AS_Move(String_format("mov `d0, %d\n", expr->u.CONST),
                    Temp_TempList(r, NULL), NULL));
            return r;
        }
        break;
        case T_CALL:
        {
            Temp_temp r = munchExp(expr->u.CALL.fun);
            Temp_tempList args = munchArgs(0, expr->u.CALL.args, F_Formals(CODEGEN_frame));
            emit(AS_Oper("call `s0\n", NULL/*F_caller_saver()*/, Temp_TempList(r, args), NULL));
            return r;
        }
        break;
        default:
            assert(0);
    }
}

static Temp_tempList munchArgs(uint32_t n, T_expList eList, F_accessList formals)
{
    return Temp_TempList(Temp_newtemp(), NULL);
}

AS_instrList F_codegen(F_frame f, T_stmList stmList)
{
    AS_instrList asList = NULL;
    T_stmList sList = stmList;
    CODEGEN_frame = f;
    for(; sList != NULL; sList = sList->tail)
    {
        munchStm(sList->head);
    }
    asList = instrList;
    instrList = instrListLast = NULL;
    return asList;
}