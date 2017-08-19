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
class parseTest : public testing::Test
{
public:
	void get() { codes = lex(code); }
	std::string code;
	std::vector<token> codes;
};


//class simpleParseTest : public parseTest
//{
//};
//TEST_F(simpleParseTest, testBaseLiteralParse)
//{
//	code = "let";
//	auto basic_expr = parse(get());
//	// TODO: some error here. Process this later...
//}

struct pSatParseTest : public parseTest
{
	void init()
	{
		code = "let";
		get();
	}
};
TEST_F(pSatParseTest, pSatParseIsString)
{
	init();
	pSat<std::string> isString([](std::string str) {return true; });
	
	auto& res = isString(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ("let", res.at(0).first);
	ASSERT_EQ(0, res.at(0).second.size());
}

struct pLitParseTest : public parseTest
{
	void init()
	{
		code = "let";
		get();
	}
	void init1()
	{
		code = "123 let";
		get();
	}
};
TEST_F(pLitParseTest, testpLitParseFunc)
{
	init();
	pLit<std::string> isLet("let");

	auto i = isLet(codes);

	ASSERT_EQ(1, i.size());
	ASSERT_EQ(code, i[0].first);

	//-------------
	init1();
	pLit<std::string> is123("123");

	i = is123(codes);

	ASSERT_EQ(1, i.size());
	ASSERT_EQ("123", i[0].first);

	//-------------
	i = isLet(i[0].second);

	ASSERT_EQ(1, i.size());
	ASSERT_EQ("let", i[0].first);
}

struct pApplyParseTest : public parseTest
{
	void init()
	{
		code = "123";
		get();
	}
};
TEST_F(pApplyParseTest, testpApplyParseInt)
{
	init();
	pVar isVar;
	pApply<std::string, int> toInt(isVar, [](std::string str) {return stoi(str); });

	auto res = toInt(codes);

	ASSERT_EQ(1, res.size());
}

struct pVarParseTest : public parseTest
{
	void init()
	{
		code = "notKeyword";
		get();
	}
	void init1()
	{
		code = "let";
		get();
	}
	void init2()
	{
		code = "+";
		get();
	}
};
TEST_F(pVarParseTest, testpVarParseForVariable)
{
	init();
	pVar isVar;
	auto res = isVar(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(code, res[0].first);
	ASSERT_EQ(0, res[0].second.size());


	init1();
	res = isVar(codes);
	ASSERT_EQ(0, res.size());

	init2();
	res = isVar(codes);
	ASSERT_EQ(0, res.size());
}

struct pNumParseTest : public parseTest
{
	void init()
	{
		code = "123";
		get();
	}
	void init1()
	{
		code = "let";
		get();
	}
};
TEST_F(pNumParseTest, testpNumParseForInt)
{
	init();
	pNum isNum;
	auto res = isNum(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(123, res[0].first);
	ASSERT_EQ(0, res[0].second.size());

	init1();
	res = isNum(codes);
	ASSERT_EQ(0, res.size());
}

//class pOrParseTest : public parseTest
//{
//};
//TEST_F(pOrParseTest, testpOrParseKeyword)
//{
//	code = "let";
//	auto codes = get();
//	
//	auto res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
//		{
//			return pLit(tokens, "let");
//		}, codes);
//
//	ASSERT_EQ(res.size(), 1);
//	ASSERT_EQ(res[0].first, std::string("let"));
//
//	//----------------------------
//	res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
//		{
//			return pLit(tokens, "hello");
//		}, codes);
//
//	ASSERT_EQ(res.size(), 0);
//
//	// --------------------------
//	code = "hello";
//	codes = get();
//
//	res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
//		{
//			return pLit(tokens, "let");
//		}, codes);
//
//	ASSERT_EQ(res.size(), 1);
//	ASSERT_EQ(res[0].first, std::string("hello"));
//
//	// -------------------------
//	res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
//		{
//			return pLit(tokens, "hello");
//		}, codes);
//
//	ASSERT_EQ(res.size(), 2);
//	ASSERT_EQ(res[0].first, std::string("hello"));
//	ASSERT_EQ(res[1].first, std::string("hello"));
//}
//
//class pThenParseTest : public parseTest
//{
//};
//#include"parse.h"
//TEST_F(pThenParseTest, testParserBaseClass)
//{
//	PLit<std::string> temp;
//	std::vector<token> ttt;
//	temp(ttt);
//}
//TEST_F(pThenParseTest, testpThenParse)
//{
//	code = "let me down";
//	auto codes = get();
//
//	auto res = pThen([](const std::vector<token>& tokens)
//		{
//			return pLit(tokens, "let");
//		},
//			[](const std::vector<token>& tokens)
//		{
//			return pLit(tokens, "me");
//		}, codes);
//	
//	ASSERT_EQ(res.size(), 1);
//	ASSERT_EQ(res[0].first.first, std::string("let"));
//	ASSERT_EQ(res[0].first.second, std::string("me"));
//	ASSERT_EQ(res[0].second.size(), 1);
//	ASSERT_EQ(res[0].second[0], std::string("down"));
//}