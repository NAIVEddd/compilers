#pragma once
#include"temp.h"
#include"../chap4/util.h"

typedef struct F_frame_ * F_frame;
typedef struct F_access_ * F_access;

typedef struct F_accessList_ * F_accessList;
struct F_accessList_ {F_access head; F_accessList tail;};

F_frame F_NewFrame(Temp_label name, U_boolList formals);

Temp_label F_Name(F_frame f);
F_acessList F_Formals(F_frame f);
F_access F_AllocLocal(F_frame f, bool escape);