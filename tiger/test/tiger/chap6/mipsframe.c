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