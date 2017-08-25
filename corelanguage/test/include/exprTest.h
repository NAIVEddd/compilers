#pragma once
#include"lex.h"
#include"parse.h"
#include"syntax.h"
#include<algorithm>
#include<cctype>
#include<functional>
#include<string>
#include<gtest/gtest.h>

/**base test class*/
class exprParseTest : public testing::Test
{
public:
	void get() { codes = lex(code); }
	std::string code;
	std::vector<token> codes;
};


struct pAltsParseTest : public exprParseTest
{
	void init()
	{
		code = "<1>hello ->; <2>world ;";
		get();
	}
};
TEST_F(pAltsParseTest, pAltsParseIsString)
{
	init();
	pExpr expr;
	pAlts getAlts;
	auto res = getAlts(codes);


}