#include"env.h"

E_enventry E_VarEntry(Ty_ty ty)
{
    E_enventry p = checked_malloc(sizeof(struct E_enventry_));
    p->kind = E_varEntry;
    p->u.var.ty = ty;
    return p;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result)
{
    E_enventry p = checked_malloc(sizeof(struct E_enventry_));
    p->kind = E_funEntry;
    p->u.fun.formals = formals;
    p->u.fun.result = result;
    return p;
}

S_table E_base_tenv(void)
{
    return TAB_empty();
}

S_table E_base_venv(void)
{
    return TAB_empty();
}