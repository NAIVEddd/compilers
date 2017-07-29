#include "stdafx.h"
#include "CppUnitTest.h"
#include"token.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			token test;
			test.name = "hh";
		}

		TEST_METHOD(TestMethod2)
		{
			std::stringstream s("hello");
			auto result = lexer(s);
			if (result.size() == 1)
			{
				std::cout << "successed";
			}
		}
	};
}

int main()
{
	test::UnitTest1 some_test;
	return 0;
}