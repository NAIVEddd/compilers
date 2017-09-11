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

pLambda::result_t pLambda::operator()(std::vector<token>& prog)
{
	pLit<std::string> isBackSlish("\\");
	pVar isVar;
	pOneOrMore<std::string> getVers(isVar);
	pLit<std::string> isSlipeSym("->");
	pExpr getRestExpr;
	pThen4<std::string, std::vector<std::string>, std::string, std::shared_ptr<expr>, typename result_t::value_type::first_type>
		parseLambda(isBackSlish,getVers,isSlipeSym,getRestExpr,
			[](std::string, std::vector<std::string> vars, std::string, std::shared_ptr<expr> body)->result_t::value_type::first_type
	{
		return std::make_shared<ELam>(vars, body);
	});
	return std::move(parseLambda(prog));
}

pCase::result_t pCase::operator()(std::vector<token>& prog)
{
	pLit<std::string> getCase("case");
	pExpr getExpr;
	pLit<std::string> getOf("of");
	pAlts getAlts;
	pThen4<std::string, std::shared_ptr<expr>, std::string, std::vector<std::shared_ptr<EAlter>>,std::shared_ptr<ECase>>
		parseCase(getCase,getExpr,getOf,getAlts,
			[](std::string, std::shared_ptr<expr>& exprs, std::string, std::vector<std::shared_ptr<EAlter>> alters) -> std::shared_ptr<ECase>
	{
		return std::make_shared<ECase>(exprs, alters);
	});
	return std::move(parseCase(prog));
}

pLet::result_t pLet::operator()(std::vector<token>& prog)
{
	pLit<std::string> getLet("let");
	pLit<std::string> getLetRec("letrec");
	pOr<std::string> letOrletrec(getLet, getLetRec);
	pDefs getDefs;
	pLit<std::string> getIn("in");
	pExpr getExpr;
	pThen4<std::string, std::vector<std::pair<std::string, std::shared_ptr<expr>>>,std::string,std::shared_ptr<expr>,std::shared_ptr<ELet>>
		parseLet(letOrletrec,getDefs,getIn,getExpr,
			[](std::string letletrec, std::vector<std::pair<std::string, std::shared_ptr<expr>>> defines, std::string, std::shared_ptr<expr> exprs)
	{
		return std::make_shared<ELet>(letletrec == "letrec", defines, exprs);
	});
	return std::move(parseLet(prog));
}

pAExpr::result_t 
pAExpr::operator()(std::vector<token>& prog)
{
	pVar pvar;
	pApply<std::string, std::shared_ptr<expr>> first_or(pvar, [](std::string var)
		{
			return std::make_shared<EVar>(var);
		});
	
	pNum pnum;
	pApply<int, std::shared_ptr<expr>> second_or(pnum, [](int num)
		{
			return std::make_shared<ENum>(num);
		});

	pConstr third_or;

	pLit<std::string> lp("(");
	pLit<std::string> rp(")");
	pExpr parseExpr;
	pThen3<std::string, std::shared_ptr<expr>, std::string, std::shared_ptr<expr>>
		forth_or(lp, parseExpr, rp, [](std::string, std::shared_ptr<expr> res, std::string)
		{
			return std::move(res);
		});

	pOr<std::shared_ptr<expr>>
		or_tmp1(first_or, second_or);
	pOr<std::shared_ptr<expr>>
		or_tmp2(or_tmp1, third_or);
	pOr<std::shared_ptr<expr>>
		orExpr(or_tmp2, forth_or);

	return orExpr(prog);
}

pConstr::result_t 
pConstr::operator()(std::vector<token>& prog)
{
	pLit<std::string> pack("pack");
	pLit<std::string> lp("{");
	pNum num;
	pThen3<std::string, std::string, int, int>
	getTag(pack, lp, num, [](std::string, std::string, int tag) 
		{
			return tag;
		});

	pLit<std::string> comon(",");
	pLit<std::string> rp("}");
	pThen4<int,std::string,int,std::string,std::shared_ptr<expr>>
	getConstr(getTag, comon, num, rp, [](int tag, std::string, int arity, std::string)
		{
			return std::make_shared<EConstr>(tag, arity);
		});
	return getConstr(prog);
}
