#include "testCase.h"
#include "./tiger/chap4/symbol.h"

// the tiger func f.
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
    return A_Fundec(0, S_Symbol("f"),
                    A_FieldList(A_Field(0, S_Symbol("a"), S_Symbol("int")),
                                A_FieldList(A_Field(0, S_Symbol("b"), S_Symbol("int")),
                                            A_FieldList(A_Field(0, S_Symbol("c"), S_Symbol("int")), NULL))),
                    S_Symbol("void"),
                    A_ExpList(A_CallExp(0, S_Symbol("print_int"), A_ExpList(A_OpExp(0, A_plusOp, A_SimpleVar(0, S_Symbol("a")), A_SimpleVar(0, S_Symbol("c"))), NULL)),
                              A_ExpList(A_LetExp(0, A_DecList(A_VarDec(0, S_Symbol("j"), S_Symbol("var"), A_OpExp(0, A_plusOp, A_SimpleVar(0, S_Symbol("a")), A_SimpleVar(0, S_Symbol("b")))), A_DecList(A_VarDec(0, A_SimpleVar(0, S_Symbol("a")), S_Symbol("var"), A_StringExp(0, "hello")), NULL)),
                                                 A_ExpList(A_CallExp(0, S_Symbol("print"), A_SimpleVar(0, S_Symbol("a"))), A_ExpList(A_CallExp(0, S_Symbol("print_int"), A_SimpleVar(0, S_Symbol("j"))), NULL))),
                                        A_CallExp(0, S_Symbol("print_int"), A_ExpList(A_SimpleVar(0, S_Symbol("b")), NULL)), NULL),
                              NULL));
}
