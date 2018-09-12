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
    A_var v_N = A_SimpleVar(1, s_N);
    A_exp ei_0 = A_IntExp(2, 0);
    A_exp ei_1 = A_IntExp(3, 1);

    A_dec dv_N = A_VarDec(4, s_N, NULL, A_IntExp(5, 8));
    A_dec dt_intArray = A_TypeDec(6, A_NametyList(A_Namety(s_intArray, A_ArrayTy(7, s_int)), NULL));
    A_dec dv_row = A_VarDec(8, s_row, NULL, A_ArrayExp(9, s_intArray, A_VarExp(10, v_N), ei_0));
    A_dec dv_col = A_VarDec(11, s_col, NULL, A_ArrayExp(12, s_intArray, A_VarExp(13, v_N), ei_0));
    A_exp op_nnm1 = A_OpExp(14, A_minusOp, A_OpExp(15, A_plusOp, A_VarExp(16, v_N), A_VarExp(17, v_N)), A_IntExp(18, 1));
    A_dec dv_diag1 = A_VarDec(19, s_diag1, NULL, A_ArrayExp(20, s_intArray, op_nnm1, ei_0));
    A_dec dv_diag2 = A_VarDec(21, s_diag2, NULL, A_ArrayExp(22, s_intArray, op_nnm1, ei_0));

/*
    function printboard() =
       (for i := 0 to N-1
	 do (for j := 0 to N-1 
	      do print(if col[i]=j then " O" else " .");
	     print("\n"));
         print("\n"))
*/
    A_exp op_nm1 = A_OpExp(23, A_minusOp, A_VarExp(24, v_N), ei_1);
    A_exp eif_print = A_IfExp(25, A_OpExp(26, A_eqOp, A_VarExp(27, A_SubscriptVar(28, A_SimpleVar(29, s_col), A_VarExp(30, A_SimpleVar(31, s_i)))), A_VarExp(32, A_SimpleVar(33, s_j))),
        A_StringExp(34, " O"), A_StringExp(35, " ."));
    A_exp ecall_print = A_CallExp(36, s_print, A_ExpList(eif_print, NULL));
    A_exp efor_printboardJ = A_ForExp(37, s_j, ei_0, op_nm1, ecall_print);
    A_exp efor_printboardI = A_ForExp(38, s_i, ei_0, op_nm1, A_SeqExp(39, A_ExpList(efor_printboardJ, A_ExpList(A_CallExp(40, s_print, A_ExpList(A_StringExp(41, "\\n"), NULL)), NULL))));
    A_fundec df_printboard = A_Fundec(42, s_printboard, NULL, S_Symbol("void"), efor_printboardI);

/*
    function try(c:int) = 
( //*  for i:= 0 to c do print("."); print("\n"); flush();*
     if c=N
     then printboard()
     else for r := 0 to N-1
	   do if row[r]=0 & diag1[r+c]=0 & diag2[r+7-c]=0
	           then (row[r]:=1; diag1[r+c]:=1; diag2[r+7-c]:=1;
		         col[c]:=r;
	                 try(c+1);
			 row[r]:=0; diag1[r+c]:=0; diag2[r+7-c]:=0)
*/

    // 'c = N'
    A_exp op_cN = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_N)));
    // 'row[r] = 0'
    A_exp op_rowr_0 = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_row), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)))), ei_0);
    // 'r + c'
    A_exp op_rpc = A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)));
    // 'diag1[r + c] = 0'
    A_exp op_diagrc_0 = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag1), op_rpc)), ei_0);
    // 'r + 7 - c'
    A_exp op_r7c = A_OpExp(++nilPos, A_minusOp, A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)), A_IntExp(++nilPos, 7)), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)));
    //'diag2[r + 7 - c] = 0'
    A_exp op_diagr7c0 = A_OpExp(++nilPos, A_eqOp, A_VarExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag2), op_r7c)), ei_0);

    // 'row[r] := 1'
    A_exp eass_row1 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_row), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r))), ei_1);
    // 'diag1[r + c] := 1'
    A_exp eass_diagrc1 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag1), op_rpc), ei_1);
    // 'diag2[r+7-c]:=1'
    A_exp eass_diagr7c1 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag2), op_r7c), ei_1);
    // 'col[c] := r'
    A_exp eass_colr = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_col), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c))), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r)));
    // 'try(c+1)'
    A_exp ecall_tryrp1 = A_CallExp(++nilPos, s_try, A_ExpList(A_OpExp(++nilPos, A_plusOp, A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_c)), A_IntExp(++nilPos, 1)), NULL));    
    // 'row[r] := 0'
    A_exp eass_row0 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_row), A_VarExp(++nilPos, A_SimpleVar(++nilPos, s_r))), ei_0);    
    // 'diag1[r+c]:=0'
    A_exp eass_diagrc0 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag1), op_rpc), ei_0);
    // 'diag2[r+7-c]:=0'
    A_exp eass_diagr7c0 = A_AssignExp(++nilPos, A_SubscriptVar(++nilPos, A_SimpleVar(++nilPos, s_diag2), op_r7c), ei_0);

    A_expList thenBody = A_ExpList(eass_row1,
            A_ExpList(eass_diagrc1, 
                A_ExpList(eass_diagr7c1,
                    A_ExpList(eass_colr, 
                        A_ExpList(ecall_tryrp1,
                            A_ExpList(eass_row0,
                                A_ExpList(eass_diagrc0,
                                    A_ExpList(eass_diagr7c0, NULL))))))));
    A_exp eif_try2 = A_IfExp(0, op_rowr_0, A_IfExp(0, op_diagrc_0, A_IfExp(0, op_diagr7c0, A_SeqExp(0, thenBody), NULL), NULL), NULL);
    A_exp efor_tryR = A_ForExp(++nilPos, s_r, ei_0, op_nm1, eif_try2);
    A_exp eif_try1 = A_IfExp(++nilPos, op_cN, A_CallExp(++nilPos, s_printboard, NULL), efor_tryR);
    A_fundec df_try = A_Fundec(++nilPos, s_try, A_FieldList(A_Field(++nilPos, s_c, s_int),NULL), S_Symbol("void"), eif_try1);

    return A_LetExp(++nilPos, A_DecList(dv_N, A_DecList(dt_intArray, A_DecList(dv_row, A_DecList(dv_col, A_DecList(dv_diag1, A_DecList(dv_diag2, A_DecList(A_FunctionDec(++nilPos, A_FundecList(df_printboard, A_FundecList(df_try, NULL))), NULL))))))),
        A_CallExp(++nilPos, s_try, A_ExpList(A_IntExp(++nilPos, 0), NULL)));
}
