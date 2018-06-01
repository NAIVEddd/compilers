#include "testCase.h"
#include "./tiger/chap4/symbol.h"

// the tiger func f.
//
// function f(a:int, b:int, c:int) = {
//     print_int(a + c);
//     let var j := a + b
//         var a := "hello"
//       in print(a); print_int(j)
//     end;
//     print_int(b);
// }
A_fundec funcF()
{
    A_pos nilPos = 0;
    S_symbol smb_f = S_Symbol("f");
    S_symbol smb_a = S_Symbol("a");
    S_symbol smb_b = S_Symbol("b");
    S_symbol smb_c = S_Symbol("c");
    S_symbol smb_j = S_Symbol("j");
    S_symbol smb_int = S_Symbol("int");
    S_symbol smb_var = S_Symbol("var");
    S_symbol smb_void = S_Symbol("void");
    S_symbol smb_print = S_Symbol("print");
    S_symbol smb_print_int = S_Symbol("print_int");

    A_field field_a = A_Field(nilPos, smb_a, smb_int);
    A_field field_b = A_Field(nilPos, smb_b, smb_int);
    A_field field_c = A_Field(nilPos, smb_c, smb_int);
    A_fieldList func_f_params = A_FieldList(field_a, A_FieldList(field_b, A_FieldList(field_c, NULL)));

    A_exp a_plus_c = A_OpExp(nilPos, A_plusOp, A_SimpleVar(nilPos, smb_a), A_SimpleVar(nilPos, smb_c));
    A_exp call_print_int = A_CallExp(nilPos, smb_print_int, A_ExpList(a_plus_c, NULL));

    A_dec dec_j = A_VarDec(nilPos, smb_j, smb_var, A_OpExp(nilPos, A_plusOp, A_SimpleVar(nilPos, smb_a), A_SimpleVar(nilPos, smb_b)));
    A_dec dec_a = A_VarDec(nilPos, A_SimpleVar(nilPos, smb_a), smb_var, A_StringExp(nilPos, "hello"));
    A_exp call_print_a = A_CallExp(nilPos, smb_print, A_SimpleVar(nilPos, smb_a));
    A_exp call_print_int_j = A_CallExp(nilPos, smb_print_int, A_SimpleVar(nilPos, smb_j));
    A_exp call_print_int_b = A_CallExp(nilPos, smb_print_int, A_ExpList(A_SimpleVar(nilPos, smb_b), NULL));

    return A_Fundec(nilPos, smb_f, func_f_params, smb_void,
                    A_ExpList(call_print_int,
                              A_ExpList(A_LetExp(nilPos, A_DecList(dec_j, A_DecList(dec_a, NULL)),
                                                 A_ExpList(call_print_a,
                                                           A_ExpList(call_print_int_j,
                                                                     NULL))),
                                        A_ExpList(call_print_int_b, NULL)),
                              NULL));
}
