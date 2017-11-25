#pragma once
#include"lex.h"
#include"syntax.h"
#include"operatorExpr.h"
#include"templateInstantiation/templateInstantiation.h"
#include<algorithm>
#include<cctype>
#include<functional>
#include<string>
#include<gtest/gtest.h>

std::string prog1 = R"(main = 1 + 2)";

class exprCompileTest : public testing::Test
{
public:
	TiState compiler(std::string& prog)
	{
		auto codes = lex(prog);
		pProgram parse;
		auto pars_res = parse(codes)[0].first;	// should be smart pointer.
		return compile(pars_res);
	}

};

TEST_F(exprCompileTest, pProgramCompileTest)
{
	auto state = compiler(prog1);

	//ASSERT_EQ(1, res.size());
	//ASSERT_EQ(2, res[0].first.size());
}

/**base test class*/
//class exprParseTest : public testing::Test
//{
//public:
//	void get() { codes = lex(code); }
//	std::string code;
//	std::vector<token> codes;
//};
//
//
//struct pAltsParseTest : public exprParseTest
//{
//	void init()
//	{
//		code = "<1>hello -> a + b; <2>world -> a - b";
//		get();
//	}
//
//	void init1()
//	{
//		code = "<1>hello -> a + b; \\ a b c -> lam";
//		get();
//	}
//};
//TEST_F(pAltsParseTest, pAltsParseIsString)
//{
//	init();
//	pAlts getAlts;
//	auto res = getAlts(codes);
//
//	ASSERT_EQ(1, res.size());
//	ASSERT_EQ(2, res[0].first.size());
//
//	init1();
//	res = getAlts(codes);
//
//	ASSERT_EQ(1, res.size());
//	ASSERT_EQ(1, res[0].first.size());
//}