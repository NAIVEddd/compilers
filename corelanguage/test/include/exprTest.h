#pragma once
#include"lex.h"
#include"parse.h"
#include"syntax.h"
#include"operatorExpr.h"
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
		code = "<1>hello -> a + b; <2>world -> a - b";
		get();
	}

	void init1()
	{
		code = "<1>hello -> a + b; \\ a b c -> lam";
		get();
	}
};
TEST_F(pAltsParseTest, pAltsParseIsString)
{
	init();
	pAlts getAlts;
	auto res = getAlts(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(2, res[0].first.size());
	
	init1();
	res = getAlts(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(1, res[0].first.size());
}

struct pELambdaParseTest : public exprParseTest
{
	void init()
	{
		code = R"(\ a b c -> laa)";
		get();
	}
};
TEST_F(pELambdaParseTest, pELambdaParse)
{
	init();
	pLambda getLambda;
	auto res = getLambda(codes);

	ASSERT_EQ(1, res.size());
}

struct pECaseParseTest : public exprParseTest
{
	void init()
	{
		code = R"(case a + b of <1> -> 2 ; <2> a - b -> 5)";
		get();
	}
};
TEST_F(pECaseParseTest, pECaseParse)
{
	init();
	pCase getCase;
	auto res = getCase(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(9, res[0].second.size());
}

struct pELetParseTest : public exprParseTest
{
	void init()
	{
		code = R"(let y = x in y)";
		get();
	}
};
TEST_F(pELetParseTest, pECaseParse)
{
	init();
	pLet getLet;
	auto res = getLet(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(0, res[0].second.size());
}

struct pFoundOpParseTest : public exprParseTest
{
	void init()
	{
		code = R"(x + y)";
		get();
	}
	void init1()
	{
		code = R"(x * y + a - b)";
		get();
	}

	void init2()
	{
		code = R"(x == y)";
		get();
	}

	void init3()
	{
		code = R"(x & y | z)";
		get();
	}
};
TEST_F(pFoundOpParseTest, pECaseParse)
{
	pExpr_p1 p1;

	init1();
	auto ress = p1(codes);

	init2();
	ress = p1(codes);

	init3();
	ress = p1(codes);
}

struct pEConstrParseTest : public exprParseTest
{
	void init()
	{
		code = R"(pack { 1, 2 })";
		get();
	}
};
TEST_F(pEConstrParseTest, pECaseParse)
{
	init();
	pConstr getConstr;
	auto res = getConstr(codes);

	EConstr pair12(1, 2);
	auto ptr = *(std::dynamic_pointer_cast<EConstr>(res.at(0).first));
	ASSERT_EQ(1,res.size());
	ASSERT_EQ(0, res.at(0).second.size());
}

struct pScDecParseTest : public exprParseTest
{
	void init()
	{
		code = R"(a = x + y)";
		get();
	}
};
TEST_F(pScDecParseTest, pECaseParse)
{
	init();
	pScDef getScDef;
	auto res = getScDef(codes);

	ASSERT_EQ(1, res.size());
}

struct pProgramParseTest : public exprParseTest
{
	void init()
	{
		code = R"(a c d = x + y; b c d = c + d)";
		get();
	}
};
TEST_F(pProgramParseTest, pECaseParse)
{
	init();
	pProgram getProgram;
	auto res = getProgram(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(2, res[0].first.defs.size());
	ASSERT_EQ(0, res[0].second.size());
}