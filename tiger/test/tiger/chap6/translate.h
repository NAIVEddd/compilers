#pragma once
#include"temp.h"
#include"frame.h"
#include"../chap4/util.h"

typedef struct Tr_level_ * Tr_level;
typedef struct Tr_access_ * Tr_access;

typedef struct Tr_accessList_ * Tr_accestList;
Tr_accestList Tr_AccessList(Tr_access head, Tr_accestList tail);

Tr_level Tr_Outermost();
Tr_level Tr_NewLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accestList Tr_Formals(Tr_level level);
Tr_access Tr_AllocLocal(Tr_level level, bool escape);