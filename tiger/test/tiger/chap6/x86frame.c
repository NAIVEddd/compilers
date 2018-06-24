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