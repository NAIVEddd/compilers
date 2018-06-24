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
    F_accessList list = F_Formals(level->frame)->tail;
    Tr_accessList tr_list = NULL;
    for(Tr_accessList db = tr_list; list; list= list->tail, db = db->tail)
    {
        db = checked_malloc(sizeof(struct Tr_accessList_));
        Tr_access mem = checked_malloc(sizeof(struct Tr_access_));
        mem->level = level;
        mem->access = list->head;
        db->head = mem;
        db->tail = NULL;
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