#pragma once
#include"lex.h"
#include"syntax.h"
#include"operatorExpr.h"
#include<algorithm>
#include<cctype>
#include<functional>
#include<string>
#include<gtest/gtest.h>

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