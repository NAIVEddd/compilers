#pragma once
#include<stdint.h>
#include"temp.h"
#include"../chap4/util.h"
#include"../chap7/tree.h"

typedef struct F_frame_ * F_frame;
typedef struct F_access_ * F_access;

typedef struct F_accessList_ * F_accessList;
struct F_accessList_ {F_access head; F_accessList tail;};

F_frame F_NewFrame(Temp_label name, U_boolList formals);

Temp_label F_Name(F_frame f);
F_accessList F_Formals(F_frame f);
F_access F_AllocLocal(F_frame f, bool escape);

Temp_temp F_FP(void);
extern const int F_wordSize;
T_exp F_Exp(F_access acc, T_exp framePtr);