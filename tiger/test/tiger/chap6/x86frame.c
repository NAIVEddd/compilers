#include"frame.h"

struct F_access_
{
    enum {inFrame, inReg} kind;
    union {
        int offset; //in frame
        Temp_temp reg;  // in reg
    } u;
};
static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);

struct F_frame_
{
    F_accessList items;
    uint32_t size;
    Temp_label label;
};

F_frame F_NewFrame(Temp_label name, U_boolList formals)
{
    F_frame frame = checked_malloc(sizeof(struct F_frame_));
    frame->label = name;
    frame->size = 0;

    F_accessList* forms = &frame->items;
    for(U_boolList iter = formals; iter; iter = iter->tail)
    {
        F_access local = F_AllocLocal(frame, formals->head);
        *forms = checked_malloc(sizeof(struct F_accessList_));
        (*forms)->head = local;
        (*forms)->tail = NULL;
        forms = &(*forms)->tail;
    }
    return frame;
}

Temp_label F_Name(F_frame f)
{
    return f->label;
}

F_accessList F_Formals(F_frame f)
{
    return f->items;
}

F_access F_AllocLocal(F_frame f, bool escape)
{
    F_access mem = checked_malloc(sizeof(struct F_access_));
    if(escape)
    {
        mem->kind = inFrame;
        mem->u.offset = f->size;
        f->size += 4;
    }
    else
    {
        mem->kind = inReg;
        mem->u.reg = Temp_newtemp();
    }
    
    // save the item to the frame item list.
    F_accessList* endIter = &f->items;
    for(; *endIter; endIter = &(*endIter)->tail)
    {;}
    F_accessList newEnd = checked_malloc(sizeof(struct F_accessList_));
    newEnd->head = mem;
    newEnd->tail = NULL;
    *endIter = newEnd;

    return mem;
}


Temp_temp F_FP(void)
{
    static Temp_temp fp = NULL;
    if(fp == NULL)
    {
        fp = Temp_newtemp();
    }
    return fp;
}
Temp_temp F_RV(void)
{
    static Temp_temp rv = NULL;
    if(rv == NULL)
    {
        rv = Temp_newtemp();
    }
    return rv;
}
const int F_wordSize = 4;

T_exp F_Exp(F_access acc, T_exp framePtr)
{
    T_exp exp = NULL;
    if(acc->kind == inFrame) exp = T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));
    else assert(0);     // no process if kind is inReg.
    return exp;
}

T_exp F_externalCall(string s, T_expList args)
{
    return T_Call(T_Name(Temp_namedlabel(s)), args);
}

F_frag F_StringFrag(Temp_label label, string str)
{
    F_frag frag = checked_malloc(sizeof(*frag));
    frag->kind = F_stringFrag;
    frag->u.stringg.label = label;
    frag->u.stringg.str = str;
    return frag;
}
F_frag F_ProcFrag(T_stm body, F_frame frame)
{
    F_frag frag = checked_malloc(sizeof(*frag));
    frag->kind = F_procFrag;
    frag->u.proc.body = body;
    frag->u.proc.frame = frame;
    return frag;
}
F_fragList F_FragList(F_frag head, F_fragList tail)
{
    F_fragList list = checked_malloc(sizeof(*list));
    list->head = head;
    list->tail = tail;
    return list;
}

Temp_map F_tempMap = NULL;
