#pragma once
#include"temp.h"
#include"frame.h"
#include"../chap4/util.h"
#include"../chap7/tree.h"
typedef void* Tr_exp;

typedef struct Tr_level_ * Tr_level;
typedef struct Tr_access_ * Tr_access;

typedef struct Tr_accessList_ * Tr_accessList;
struct Tr_accessList_ { Tr_access head; Tr_accessList tail;};
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

Tr_level Tr_Outermost();
Tr_level Tr_NewLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_Formals(Tr_level level);
Tr_access Tr_AllocLocal(Tr_level level, bool escape);

Tr_exp Tr_simpleVar(Tr_access , Tr_level);
Tr_exp Tr_subscriptVar(Tr_access array, Tr_level level, Tr_exp index);
Tr_exp Tr_String(string s);
Tr_exp Tr_int(int num);

Tr_exp Tr_ArrayInit(Tr_exp size, Tr_exp init);