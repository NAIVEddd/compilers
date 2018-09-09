#include"translate.h"
#include"frame.h"

struct Tr_access_ {Tr_level level; F_access access;};
struct Tr_level_ {Tr_level parent; Temp_label name; F_frame frame;};

Tr_level Tr_Outermost()
{
    static Tr_level outermost = NULL;
    if(outermost == NULL)
    {
        outermost = Tr_NewLevel(NULL, Temp_newlabel(), NULL);
    }
    return outermost;
}

Tr_level Tr_NewLevel(Tr_level parent, Temp_label name, U_boolList formals)
{
    Tr_level level = checked_malloc(sizeof(struct Tr_level_));
    level->parent = parent;
    level->name = name;
    level->frame = F_NewFrame(name, U_BoolList(TRUE, formals));


    return level;
}

Tr_accessList Tr_Formals(Tr_level level)
{
    F_accessList list = F_Formals(level->frame);
    Tr_accessList tr_list = NULL;
    for(Tr_accessList* db = &tr_list; list; list= list->tail, db = &(*db)->tail)
    {
        *db = checked_malloc(sizeof(struct Tr_accessList_));
        Tr_access mem = checked_malloc(sizeof(struct Tr_access_));
        mem->level = level;
        mem->access = list->head;
        (*db)->head = mem;
        (*db)->tail = NULL;
    }
    return tr_list;
}

Tr_access Tr_AllocLocal(Tr_level level, bool escape)
{
    Tr_access access = checked_malloc(sizeof(struct Tr_access_));
    access->access = F_AllocLocal(level->frame, escape);
    access->level = level;
    return access;
}

Tr_exp Tr_StaticLink(Tr_access access, Tr_level level)
{
    Tr_exp exp = T_Temp(F_FP());
    while(access->level != level)
    {
        exp = F_Exp(F_Formals(level->frame)->head, exp);
        level = level->parent;
    }
    return exp;
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level)
{
    return F_Exp(access->access, Tr_StaticLink(access, level));       // local var, temp impl.
}

Tr_exp Tr_subscriptVar(Tr_access array, Tr_level level, Tr_exp index)
{
    T_exp mem = F_Exp(array->access, Tr_StaticLink(array, level));
    return T_Mem(T_Binop(T_plus, mem, T_Binop(T_mul, T_Const(F_wordSize), index)));
}

Tr_exp Tr_String(string s)
{
    Temp_label lab = Temp_newlabel();
    F_StringFrag(lab, s);
    return T_Name(lab);
}

Tr_exp Tr_int(int num)
{
    return T_Const(num);
}

Tr_exp Tr_ArrayInit(Tr_exp size, Tr_exp init)
{
    return F_externalCall("initArray", T_ExpList(size, T_ExpList(init, NULL)));
}