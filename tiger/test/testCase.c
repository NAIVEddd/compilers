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

    A_field field_a = A_Field(++nilPos, smb_a, smb_int);
    A_field field_b = A_Field(++nilPos, smb_b, smb_int);
    A_field field_c = A_Field(++nilPos, smb_c, smb_int);
    A_fieldList func_f_params = A_FieldList(field_a, A_FieldList(field_b, A_FieldList(field_c, NULL)));

    A_exp a_plus_c = A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, smb_a)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, smb_c)));
    A_exp call_print_int = A_CallExp(++nilPos, smb_print_int, A_ExpList(a_plus_c, NULL));

    A_dec dec_j = A_VarDec(++nilPos, smb_j, smb_var, A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, smb_a)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, smb_b))));
    A_dec dec_a = A_VarDec(++nilPos, smb_a, smb_var, A_StringExp(++nilPos, "hello"));
    A_exp call_print_a = A_CallExp(++nilPos, smb_print, A_ExpList(A_VarExp(++nilPos, A_SimpleVar(++nilPos, smb_a)), NULL));
    A_exp call_print_int_j = A_CallExp(++nilPos, smb_print_int, A_ExpList(A_VarExp(++nilPos, A_SimpleVar(++nilPos, smb_j)), NULL));
    A_exp call_print_int_b = A_CallExp(++nilPos, smb_print_int, A_ExpList(A_VarExp(++nilPos, A_SimpleVar(++nilPos, smb_b)), NULL));

    return A_Fundec(++nilPos, smb_f, func_f_params, smb_void,
                    A_SeqExp(++nilPos, A_ExpList(call_print_int,
                                               A_ExpList(A_LetExp(++nilPos, A_DecList(dec_j, A_DecList(dec_a, NULL)),
                                                                  A_SeqExp(++nilPos, A_ExpList(call_print_a,
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
    A_dec arrtype = A_TypeDec(++nilPos, A_NametyList(A_Namety(S_Symbol("arrtype"), A_ArrayTy(++nilPos, S_Symbol("int"))), NULL));
    A_dec arr1 = A_VarDec(++nilPos, S_Symbol("arr1"), S_Symbol("arrtype"), A_ArrayExp(++nilPos, S_Symbol("arrtype"), A_IntExp(++nilPos, 10), A_IntExp(++nilPos, 0)));
    return A_LetExp(++nilPos, A_DecList(arrtype, A_DecList(arr1, NULL)),
                    A_VarExp(++nilPos, A_SimpleVar(++nilPos, S_Symbol("arr1"))));
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
    A_dec rectype = A_TypeDec(++nilPos,
                              A_NametyList(A_Namety(S_Symbol("rectype"),
                                                    A_RecordTy(++nilPos, A_FieldList(A_Field(++nilPos, S_Symbol("name"), S_Symbol("string")),
                                                                                   A_FieldList(A_Field(++nilPos, S_Symbol("age"), S_Symbol("int")),
                                                                                               NULL)))),
                                           NULL));
    A_dec rec1 = A_VarDec(++nilPos,
                          S_Symbol("rec1"),
                          S_Symbol("rectype"),
                          A_RecordExp(++nilPos,
                                      S_Symbol("rectype"),
                                      A_EfieldList(A_Efield(S_Symbol("name"), A_StringExp(++nilPos, "Nobody")),
                                                   A_EfieldList(A_Efield(S_Symbol("age"), A_IntExp(++nilPos, 1000)),
                                                                NULL))));
    A_var rec1_name = A_FieldVar(++nilPos,
                                 A_SimpleVar(++nilPos, S_Symbol("rec1")), S_Symbol("name"));
    A_exp assign = A_AssignExp(++nilPos,
                               rec1_name,
                               A_StringExp(++nilPos, "Somebody"));
    return A_LetExp(++nilPos,
                    A_DecList(rectype, A_DecList(rec1, NULL)),
                    A_SeqExp(++nilPos, A_ExpList(assign,
                                       A_ExpList(A_VarExp(++nilPos, A_SimpleVar(++nilPos, S_Symbol("rec1"))),
                                                 NULL))));
}

// prog frag from:
// compilers/tiger/test/tiger/testcases/queens.tig
A_exp MakeQueensTig()
{
    A_pos nilPos = 0;
    S_symbol s_var = S_Symbol("var");
    S_symbol s_N = S_Symbol("N");
    S_symbol s_intArray = S_Symbol("intArray");
    S_symbol s_int = S_Symbol("int");
    S_symbol s_row = S_Symbol("row");
    S_symbol s_col = S_Symbol("col");
    S_symbol s_diag1 = S_Symbol("diag1");
    S_symbol s_diag2 = S_Symbol("diag2");
    S_symbol s_i = S_Symbol("i");
    S_symbol s_j = S_Symbol("j");
    S_symbol s_c = S_Symbol("c");
    S_symbol s_r = S_Symbol("r");
    S_symbol s_printboard = S_Symbol("printboard");
    S_symbol s_print = S_Symbol("print");
    S_symbol s_try = S_Symbol("try");
    A_var v_N = A_SimpleVar(++nilPos, s_N);
    A_exp ei_0 = A_IntExp(++nilPos, 0);
    A_exp ei_1 = A_IntExp(++nilPos, 1);

    A_dec dv_N = A_VarDec(++nilPos, s_N, NULL, A_IntExp(++nilPos, 8));
    A_dec dt_intArray = A_TypeDec(++nilPos, A_NametyList(A_Namety(s_intArray, A_ArrayTy(++nilPos, s_int)), NULL));
    A_dec dv_row = A_VarDec(++nilPos, s_row, NULL, A_ArrayExp(++nilPos, s_intArray, A_VarExp(++nilPos, v_N), ei_0));
    A_dec dv_col = A_VarDec(++nilPos, s_col, NULL, A_ArrayExp(++nilPos, s_intArray, A_VarExp(++nilPos, v_N), ei_0));
    A_exp op_nnm1 = A_OpExp(++nilPos, A_minusOp, A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, v_N), A_VarExp(++nilPos, v_N)), A_IntExp(++nilPos, 1));
    A_dec dv_diag1 = A_VarDec(++nilPos, s_diag1, NULL, A_ArrayExp(++nilPos, s_intArray, op_nnm1, ei_0));
    A_dec dv_diag2 = A_VarDec(++nilPos, s_diag2, NULL, A_ArrayExp(++nilPos, s_intArray, op_nnm1, ei_0));

    A_exp op_nm1 = A_OpExp(++nilPos, A_minusOp, A_VarExp(++nilPos, v_N), ei_1);
    A_exp eif_print = A_IfExp(++nilPos, A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_col), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_i)))), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_j))),
        A_StringExp(++nilPos, " O"), A_StringExp(++nilPos, " ."));
    A_exp ecall_print = A_CallExp(++nilPos, s_print, A_ExpList(eif_print, NULL));
    A_exp efor_printboardJ = A_ForExp(++nilPos, s_j, ei_0, op_nm1, ecall_print);
    A_exp efor_printboardI = A_ForExp(++nilPos, s_i, ei_0, op_nm1, A_SeqExp(++nilPos, A_ExpList(efor_printboardJ, A_ExpList(A_CallExp(++nilPos, s_print, A_ExpList(A_StringExp(++nilPos, "\\n"), NULL)), NULL))));
    A_fundec df_printboard = A_Fundec(++nilPos, s_printboard, NULL, S_Symbol("void"), efor_printboardI);

    A_exp op_cN = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_N)));
    A_exp op_rowr_0 = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_row), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)))), ei_0);
    A_exp op_rpc = A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)));
    A_exp op_diagrc_0 = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag1), op_rpc)), ei_0);
    A_exp op_r7c = A_OpExp(++nilPos, A_minusOp, A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)), A_IntExp(++nilPos, 7)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)));
    A_exp op_diagr7c0 = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag2), op_r7c)), ei_0);
    A_exp eif_AllTrue = A_IfExp(++nilPos, op_rowr_0, A_IfExp(++nilPos, op_diagrc_0, op_diagr7c0, A_IntExp(++nilPos, 0)), A_IntExp(++nilPos, 0));

    A_exp eass_row1 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_row), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r))), ei_1);
    A_exp eass_diagrc1 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag1), op_rpc), ei_1);
    A_exp eass_diagr7c1 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag2), op_r7c), ei_1);
    A_exp eass_colr = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_col), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c))), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)));
    A_exp eass_row0 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_row), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r))), ei_0);    
    A_exp eass_diagrc0 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag1), op_rpc), ei_0);
    A_exp eass_diagr7c0 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag2), op_r7c), ei_0);
    A_exp ecall_tryrp1 = A_CallExp(++nilPos, s_try, A_ExpList(A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)), A_IntExp(++nilPos, 1)), NULL));

    A_exp eif_try2 = A_IfExp(++nilPos, eif_AllTrue, A_SeqExp(++nilPos, A_ExpList(eass_row1, A_ExpList(eass_diagrc1, A_ExpList(eass_diagr7c1, A_ExpList(eass_colr, A_ExpList(ecall_tryrp1, A_ExpList(eass_row0, A_ExpList(eass_diagrc0, A_ExpList(eass_diagr7c0, NULL))))))))), NULL);
    A_exp efor_tryR = A_ForExp(++nilPos, s_r, ei_0, op_nm1, eif_try2);
    A_exp eif_try1 = A_IfExp(++nilPos, op_cN, A_CallExp(++nilPos, s_printboard, NULL), efor_tryR);
    A_fundec df_try = A_Fundec(++nilPos, s_try, A_FieldList(A_Field(++nilPos, s_c, s_int),NULL), S_Symbol("void"), eif_try1);

    return A_LetExp(++nilPos, A_DecList(dv_N, A_DecList(dt_intArray, A_DecList(dv_row, A_DecList(dv_col, A_DecList(dv_diag1, A_DecList(dv_diag2, A_DecList(A_FunctionDec(++nilPos, A_FundecList(df_printboard, A_FundecList(df_try, NULL))), NULL))))))),
        A_CallExp(++nilPos, s_try, A_ExpList(A_IntExp(++nilPos, 0), NULL)));
}
