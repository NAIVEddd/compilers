#pragma once
#include"lex.h"
#include"parse.h"
#include"syntax.h"
#include"syntax.cpp"
#include<algorithm>
#include<cctype>
#include<functional>
#include<string>
#include<gtest/gtest.h>

/**base test class*/
class parseTest : public testing::Test
{
public:
	std::vector<token> get() const { return lex(code); }
	std::string code;
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

class pLitParseTest : public parseTest
{
};
TEST_F(pLitParseTest, testpLitParseFunc)
{
	code = "let";
	auto codes = get();
	auto i = pLit(codes, "let");
	ASSERT_EQ(i.size(), 1);
	ASSERT_EQ(i[0].first, code);

	code = "123 let";
	codes = get();
	i = pLit(codes, "123");
	ASSERT_EQ(i.size(), 1);
	ASSERT_EQ(i[0].first, "123");
	i = pLit(i[0].second, "let");
	ASSERT_EQ(i.size(), 1);
	ASSERT_EQ(i[0].first, "let");
}

class pNumParseTest : public parseTest
{
};
TEST_F(pNumParseTest, testpNumParseForInt)
{
	code = "123";
	auto codes = get();
	auto res = pNum(codes);
	ASSERT_EQ(res.size(), 1);
	ASSERT_EQ(res[0].first, 123);
	ASSERT_EQ(res[0].second.size(), 0);

	code = "let";
	codes = get();
	res = pNum(codes);
	ASSERT_EQ(res.size(), 0);
}

class pVarParseTest : public parseTest
{
};
TEST_F(pVarParseTest, testpVarParseForVariable)
{
	code = "notKeyword";
	auto codes = get();
	auto res = pVar(codes);
	ASSERT_EQ(res.size(), 1);
	ASSERT_EQ(res[0].first, code);
	ASSERT_EQ(res[0].second.size(), 0);

	code = "let";
	codes = get();
	res = pVar(codes);
	ASSERT_EQ(res.size(), 0);
}

class pOrParseTest : public parseTest
{
};
TEST_F(pOrParseTest, testpOrParseKeyword)
{
	code = "let";
	auto codes = get();
	
	auto res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
		{
			return pLit(tokens, "let");
		}, codes);

	ASSERT_EQ(res.size(), 1);
	ASSERT_EQ(res[0].first, std::string("let"));

	//----------------------------
	res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
		{
			return pLit(tokens, "hello");
		}, codes);

	ASSERT_EQ(res.size(), 0);

	// --------------------------
	code = "hello";
	codes = get();

	res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
		{
			return pLit(tokens, "let");
		}, codes);

	ASSERT_EQ(res.size(), 1);
	ASSERT_EQ(res[0].first, std::string("hello"));

	// -------------------------
	res = pOr<tokens_list, decltype(pVar)>(pVar, [](const std::vector<token>& tokens)
		{
			return pLit(tokens, "hello");
		}, codes);

	ASSERT_EQ(res.size(), 2);
	ASSERT_EQ(res[0].first, std::string("hello"));
	ASSERT_EQ(res[1].first, std::string("hello"));
}