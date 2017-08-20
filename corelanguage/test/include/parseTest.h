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

struct pOrParseTest : public parseTest
{
	void init()
	{
		code = "let";
		get();
	}
	void init1()
	{
		code = "hello";
		get();
	}
};
TEST_F(pOrParseTest, testpOrParseKeyword)
{
	init();
	pVar isVar;
	pLit<std::string> isLet("let");
	pOr<std::string> varOrLet(isVar, isLet);

	auto res = varOrLet(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(std::string("let"), res[0].first);

	//----------------------------
	pLit<std::string> isHello("hello");
	pOr<std::string> varOrHello(isVar, isHello);
	res = varOrHello(codes);

	ASSERT_EQ(0, res.size());

	// --------------------------
	init1();

	res = varOrLet(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(std::string("hello"), res[0].first);

	// -------------------------
	res = varOrHello(codes);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ(std::string("hello"), res[0].first);
	ASSERT_EQ(std::string("hello"), res[1].first);
}

struct pThenParseTest : public parseTest
{
	void init()
	{
		code = "let me down";
		get();
	}
	void init1()
	{
		code = "equal 11";
		get();
	}
};
TEST_F(pThenParseTest, testpThenParse)
{
	init();
	pLit<std::string> isLet("let");
	pLit<std::string> isMe("me");
	pThen<std::string, std::string, std::pair<std::string, std::string>> str_pair(isLet, isMe, std::make_pair<std::string,std::string>);

	auto res = str_pair(codes);
	
	ASSERT_EQ(1, res.size());
	ASSERT_EQ(std::string("let"), res[0].first.first);
	ASSERT_EQ(std::string("me"), res[0].first.second);
	ASSERT_EQ(1, res[0].second.size());
	ASSERT_EQ(std::string("down"), res[0].second[0].name);

	init1();
	pLit<std::string> isEqual("equal");
	pNum isNum;
	pThen <std::string, int, std::pair<std::string, int>> str_int(isEqual, isNum, std::make_pair<std::string, int>);

	auto res1 = str_int(codes);

	ASSERT_EQ(1, res1.size());
	ASSERT_EQ(std::string("equal"), res1[0].first.first);
	ASSERT_EQ(11, res1[0].first.second);
}

struct pThen3ParseTest : public parseTest
{
	void init()
	{
		code = "let me down";
		get();
	}
};
TEST_F(pThen3ParseTest, testpThen3Parse)
{
	init();
	pLit<std::string> isLet("let");
	pLit<std::string> isMe("me");
	pLit<std::string> isDown("down");
	pThen3<std::string, std::string, std::string, std::tuple<std::string, std::string, std::string>> tribleStr(isLet, isMe, isDown, std::make_tuple<std::string, std::string, std::string>);

	auto& res = tribleStr(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(std::string("let"), std::get<0>(res[0].first));
	ASSERT_EQ(std::string("me"), std::get<1>(res[0].first));
	ASSERT_EQ(std::string("down"), std::get<2>(res[0].first));
	ASSERT_EQ(0, res[0].second.size());
}

struct pThen4ParseTest : public parseTest
{
	void init()
	{
		code = "let me down!";
		get();
	}
};
TEST_F(pThen4ParseTest, testpThen3Parse)
{
	init();
	pLit<std::string> isLet("let");
	pLit<std::string> isMe("me");
	pLit<std::string> isDown("down");
	pLit<std::string> isPunct("!");
	pThen4<std::string, std::string, std::string, std::string, std::tuple<std::string, std::string, std::string, std::string>> fourStr(isLet, isMe, isDown, isPunct, std::make_tuple<std::string, std::string, std::string, std::string>);

	auto& res = fourStr(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(std::string("let"), std::get<0>(res[0].first));
	ASSERT_EQ(std::string("me"), std::get<1>(res[0].first));
	ASSERT_EQ(std::string("down"), std::get<2>(res[0].first));
	ASSERT_EQ(std::string("!"), std::get<3>(res[0].first));
	ASSERT_EQ(0, res[0].second.size());
}

struct pEmptyParseTest : public parseTest
{
	void init()
	{
		code = "let me down";
		get();
	}
};
TEST_F(pEmptyParseTest, testpThenParse)
{
	init();

	pEmpty<int> defInt(0);
	auto res = defInt(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(0, res.at(0).first);

	pEmpty<std::string> defStr("empty");
	auto res1 = defStr(codes);

	ASSERT_EQ(1, res1.size());
	ASSERT_EQ(std::string("empty"), res1.at(0).first);

	pEmpty<std::string> defCons;
	auto res2 = defCons(codes);
	
	ASSERT_EQ(1, res2.size());
}

struct pOneOrMoreParseTest : public parseTest
{
	void init()
	{
		code = "let let let";
		get();
	}
	void init1()
	{
		code = "";
		get();
	}
};
TEST_F(pOneOrMoreParseTest, testpOneOrMoreParse)
{
	init();

	pLit<std::string> isLet("let");
	pOneOrMore<std::string> oneOrMore(isLet);
	auto res = oneOrMore(codes);

	ASSERT_EQ(1, res.size());
	ASSERT_EQ(3, res[0].first.size());
	ASSERT_EQ(std::string("let"), res[0].first.at(0));
	ASSERT_EQ(std::string("let"), res[0].first.at(1));
	ASSERT_EQ(std::string("let"), res[0].first.at(2));

	init1();

	res = oneOrMore(codes);

	ASSERT_EQ(0, res.size());
}

struct pZeroOrMoreParseTest : public parseTest
{
	void init()
	{
		code = "let let let";
		get();
	}
	void init1()
	{
		code = "";
		get();
	}
};
TEST_F(pZeroOrMoreParseTest, testpZeroOrMoreParse)
{
	init();

	pLit<std::string> isLet("let");
	pZeroOrMore<std::string> zeroOrMoreLet(isLet);
	auto res = zeroOrMoreLet(codes);

	ASSERT_EQ(2, res.size());
	ASSERT_EQ(3, res[0].first.size());
	ASSERT_EQ(std::string("let"), res[0].first.at(0));
	ASSERT_EQ(std::string("let"), res[0].first.at(1));
	ASSERT_EQ(std::string("let"), res[0].first.at(2));

	init1();

	pLit<std::string> isHello("hello");
	pZeroOrMore<std::string> zeroOrMoreHello(isHello);
	res = zeroOrMoreHello(codes);

	ASSERT_EQ(1, res.size());
}