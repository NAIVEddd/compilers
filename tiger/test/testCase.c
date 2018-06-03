#include "testCase.h"
#include "./tiger/chap4/symbol.h"

#ifndef NULL
#define NULL 0
#endif

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

    A_exp a_plus_c = A_OpExp(nilPos, A_plusOp, A_VarExp(nilPos, A_SimpleVar(nilPos, smb_a)), A_VarExp(nilPos, A_SimpleVar(nilPos, smb_c)));
    A_exp call_print_int = A_CallExp(nilPos, smb_print_int, A_ExpList(a_plus_c, NULL));

    A_dec dec_j = A_VarDec(nilPos, smb_j, smb_var, A_OpExp(nilPos, A_plusOp, A_VarExp(nilPos, A_SimpleVar(nilPos, smb_a)), A_VarExp(nilPos, A_SimpleVar(nilPos, smb_b))));
    A_dec dec_a = A_VarDec(nilPos, smb_a, smb_var, A_StringExp(nilPos, "hello"));
    A_exp call_print_a = A_CallExp(nilPos, smb_print, A_ExpList(A_VarExp(nilPos, A_SimpleVar(nilPos, smb_a)), NULL));
    A_exp call_print_int_j = A_CallExp(nilPos, smb_print_int, A_ExpList(A_VarExp(nilPos, A_SimpleVar(nilPos, smb_j)), NULL));
    A_exp call_print_int_b = A_CallExp(nilPos, smb_print_int, A_ExpList(A_VarExp(nilPos, A_SimpleVar(nilPos, smb_b)), NULL));

    return A_Fundec(nilPos, smb_f, func_f_params, smb_void,
                    A_SeqExp(nilPos, A_ExpList(call_print_int,
                                               A_ExpList(A_LetExp(nilPos, A_DecList(dec_j, A_DecList(dec_a, NULL)),
                                                                  A_SeqExp(nilPos, A_ExpList(call_print_a,
                                                                                             A_ExpList(call_print_int_j,
                                                                                                       NULL)))),
                                                         A_ExpList(call_print_int_b, NULL)))));
}

/* an array type and an array variable */
//
// let
// 	type  arrtype = array of int
// 	var arr1:arrtype := arrtype [10] of 0
// in
// 	arr1
// end
A_exp ArrayType()
{
    A_pos nilPos = 0;
    A_dec arrtype = A_TypeDec(nilPos, A_NametyList(A_Namety(S_Symbol("arrtype"), A_ArrayTy(nilPos, S_Symbol("int"))), NULL));
    A_dec arr1 = A_VarDec(nilPos, S_Symbol("arr1"), S_Symbol("arrtype"), A_ArrayExp(nilPos, S_Symbol("arrtype"), A_IntExp(nilPos, 10), A_IntExp(nilPos, 0)));
    return A_LetExp(nilPos, A_DecList(arrtype, A_DecList(arr1, NULL)),
                    A_VarExp(nilPos, A_SimpleVar(nilPos, S_Symbol("arr1"))));
}

/* a record type and a record variable */
//
// let
// 	type  rectype = {name:string, age:int}
// 	var rec1:rectype := rectype {name="Nobody", age=1000}
// in
// 	rec1.name := "Somebody";
// 	rec1
// end
A_exp RecordVar()
{
    A_pos nilPos = 0;
    A_dec rectype = A_TypeDec(nilPos,
                              A_NametyList(A_Namety(S_Symbol("rectype"),
                                                    A_RecordTy(nilPos, A_FieldList(A_Field(nilPos, S_Symbol("name"), S_Symbol("string")),
                                                                                   A_FieldList(A_Field(nilPos, S_Symbol("age"), S_Symbol("int")),
                                                                                               NULL)))),
                                           NULL));
    A_dec rec1 = A_VarDec(nilPos,
                          S_Symbol("rec1"),
                          S_Symbol("rectype"),
                          A_RecordExp(nilPos,
                                      S_Symbol("rectype"),
                                      A_EfieldList(A_Efield(S_Symbol("name"), A_StringExp(nilPos, "Nobody")),
                                                   A_EfieldList(A_Efield(S_Symbol("age"), A_IntExp(nilPos, 1000)),
                                                                NULL))));
    A_var rec1_name = A_FieldVar(nilPos,
                                 A_SimpleVar(nilPos, S_Symbol("rec1")), S_Symbol("name"));
    A_exp assign = A_AssignExp(nilPos,
                               rec1_name,
                               A_StringExp(nilPos, "Somebody"));
    return A_LetExp(nilPos,
                    A_DecList(rectype, A_DecList(rec1, NULL)),
                    A_SeqExp(nilPos, A_ExpList(assign,
                                       A_ExpList(A_VarExp(nilPos, A_SimpleVar(nilPos, S_Symbol("rec1"))),
                                                 NULL))));
}
