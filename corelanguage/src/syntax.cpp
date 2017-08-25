#include"base.h"
#include"syntax.h"
#include<algorithm>
#include<functional>
using prd = std::function<bool(token)>;
using tokens_list = std::vector<std::pair<std::string, std::vector<token>>>;



//tokens_list
//pSat(const std::vector<token>& tokens, prd func)
//{
//	tokens_list result;
//	if (tokens.size() == 0)
//	{
//		return result;
//	}
//	if (func(tokens[0]))
//	{
//		result.push_back(std::pair<std::string, std::vector<token>>{ tokens[0].name, std::vector<token>(++tokens.begin(), tokens.end()) });
//		return result;
//	}
//	else
//	{
//		return result;
//	}
//}

//tokens_list
//pLit(const std::vector<token>& tokens, std::string name)
//{
//	return pSat(tokens, [&name](token tok)->bool {return name == tok.name; });
//}

//using patten = std::function<tokens_list(const std::vector<token>&, prd)>;
//template<typename out, typename in>
//class transFunc
//{
//public:
//	transFunc(std::function<out(in)> func_) :func(func_) {};
//	out operator()(in para) { return func(para); }
//
//	std::function<out(in)> func;
//};

//template<typename out, typename in>
//std::vector<std::pair<out, std::vector<token>>>
//pApply(const std::vector<token>& tokens, patten p, prd prdFunc, transFunc<out,in> trans)
//{
//	std::vector<std::pair<out, std::vector<token>>> result;
//	auto list = p(tokens, prdFunc);
//	if (list.size() != 0)
//	{
//		auto newfst = trans(list[0].first);
//		result.push_back({ newfst,list[0].second });
//	}
//	return result;
//}

//tokens_list
//pVar(const std::vector<token>& tokens)
//{
//	const auto & keywordList = keywords;
//	prd isVar = [& keywordList](const token& tok) { return std::find(keywordList.cbegin(), keywordList.cend(), tok.name) == keywordList.cend(); };
//	return pSat(tokens, isVar);
//}

//std::vector<std::pair<uint32_t, std::vector<token>>>
//pNum(const std::vector<token>& tokens)
//{
//	prd isNum = [](const token& tok) { return std::all_of(tok.name.begin(), tok.name.end(), isdigit); };
//	transFunc<uint32_t, std::string> toInt([](std::string str) { return (uint32_t)std::stol(str); });
//	return pApply(tokens,
//		pSat,
//		isNum,
//		toInt);
//}

//template<typename result_type, typename parser>
//result_type
//pOr(parser p1, parser p2, const std::vector<token>& tokens)
//{
//	auto res1 = p1(tokens);
//	auto res2 = p2(tokens);
//	if (!res1.size())
//	{
//		return res2;
//	}
//	else if (!res2.size())
//	{
//		return res1;
//	}
//
//	for (auto& tok : res2)
//	{
//		res1.push_back(tok);
//	}
//	return res1;
//}

/*template<typename result_type, typename parser>
result_type
pThen(parser p1, parser pb, const std::vector<token>& tokens)
{
	auto res1 = p1(tokens);
	auto res2 = p1(res1[0].second);

	return std::make_pair((res1[0].first, res2[0].first), res2[0].second);
}*/

pAlts::pAlt::result_t pAlts::pAlt::operator()(std::vector<token>& prog)
{
	pLit<std::string> leftB("<");
	pNum midNum;
	pLit<std::string> rightB(">");
	pThen3<std::string, int, std::string, int> parseAltTag(leftB, midNum, rightB, [](std::string, int i, std::string) {return i; });

	pVar var;
	pZeroOrMore<std::string> vars(var);
	pLit<std::string> sliSymbol("->");
	pExpr getExpr;
	pThen4<int, std::vector<std::string>, std::string, std::shared_ptr<expr>, std::shared_ptr<EAlter>> getAlt(parseAltTag, vars, sliSymbol, getExpr, [](int tag, std::vector<std::string>& params, std::string, std::shared_ptr<expr>& expr) { return std::make_unique<EAlter>(tag, std::move(params), std::move(expr)); });

	return getAlt(prog);
}

pDefs::pDef::result_t pDefs::pDef::operator()(std::vector<token>& prog)
{
	pVar var;
	pLit<std::string> eqSym("=");
	pExpr getExpr;
	pThen3<std::string, std::string, std::shared_ptr<expr>, std::pair<std::string, std::shared_ptr<expr>>>
		getDef(var, eqSym, getExpr, [](std::string name, std::string, std::shared_ptr<expr>& body)->std::pair<std::string, std::shared_ptr<expr>> {return { name, std::move(body) }; });
	return getDef(prog);
}
