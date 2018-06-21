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

    F_accessList forms = frame->items;
    for(U_boolList iter = formals; iter; iter = iter->tail)
    {
        frame->size += 4;
        F_access local = F_AllocLocal(frame, formals->head);
        // forms->head = local; // buggggg
    }
}