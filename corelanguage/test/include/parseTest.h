#pragma once
#include"lex.h"
#include"parse.h"
#include"syntax.cpp"
#include<algorithm>
#include<cctype>
#include<functional>
#include<string>
#include<gtest/gtest.h>

class parseTest : public testing::Test
{
public:
	std::vector<token> get() const { return lex(code); }
	std::string code;
};

TEST_F(parseTest, testBaseLiteralParse)
{
	code = "let";
	auto basic_expr = parse(get());
}

TEST_F(parseTest, testpLitParseFunc)
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

TEST_F(parseTest, testpApplyParseForInt)
{
	code = "123";
	auto codes = get();
	prd isNum = [](token tok) { return std::all_of(tok.name.begin(), tok.name.end(), isdigit); };
	auto res = pApply(codes,
		pSat,
		isNum,
		toInt);

}