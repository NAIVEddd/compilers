#include<iostream>
#include"lex.h"
#include<gtest/gtest.h>

TEST(testing, sometest)
{
	std::string core{ "432" };
	auto res = lex(core);

	ASSERT_EQ(res.size(),1);
}

int main(int argc, char **argv)
{
	testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}