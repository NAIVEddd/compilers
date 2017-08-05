#pragma once
#include"lex.h"
#include<gtest/gtest.h>

class lexTest :public testing::Test
{
public:
	std::vector<token> tokens;
};

TEST_F(lexTest, parseSomeSimpleLiteral)
{
	std::string core{ "432\nhello < <= > >= " };
	tokens = lex(core);

	ASSERT_EQ(tokens.size(), 6);
}
