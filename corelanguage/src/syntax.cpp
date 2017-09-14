#include"base.h"
#include"syntax.h"
#include"operatorExpr.h"
#include<algorithm>
#include<functional>

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
	pThen4<std::string, std::shared_ptr<expr>, std::string, std::vector<std::shared_ptr<EAlter>>,std::shared_ptr<expr>>
		parseCase(getCase,getExpr,getOf,getAlts,
			[](std::string, std::shared_ptr<expr>& exprs, std::string, std::vector<std::shared_ptr<EAlter>> alters) -> std::shared_ptr<expr>
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
	pThen4<std::string, std::vector<std::pair<std::string, std::shared_ptr<expr>>>,std::string,std::shared_ptr<expr>,std::shared_ptr<expr>>
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

pExpr::result_t 
pExpr::operator()(std::vector<token>& prog)
{
	pLet getLet;
	pCase getCase;
	pLambda getLambda;
	pExpr_p1 p1;

	pOr<std::shared_ptr<expr>> tmp1(getLet, getCase);
	pOr<std::shared_ptr<expr>> tmp2(tmp1, getLambda);
	pOr<std::shared_ptr<expr>> getExpr(tmp2, p1);

	return getExpr(prog);
}

pScDef::result_t 
pScDef::operator()(std::vector<token>& prog)
{
	pVar getVariable;
	pZeroOrMore<std::string> getVars(getVariable);
	pLit<std::string> getEq("=");
	pExpr getExpr;
	pThen4<std::string,std::vector<std::string>,std::string,std::shared_ptr<expr>, std::shared_ptr<ScDef>>
		getScDef(getVariable, getVars, getEq, getExpr, [](std::string, std::vector<std::string>, std::string, std::shared_ptr<expr>)
			{
				return std::make_shared<ScDef>();
			});
	return getScDef(prog);
}

pProgram::result_t 
pProgram::operator()(std::vector<token>& prog)
{
	pScDef getScDef;
	pLit<std::string> getComma(";");
	pOneOrMoreWithSpt<std::shared_ptr<ScDef>, std::string>
		getProgram(getScDef, getComma);
	auto res_second = getProgram(prog);
	program res_first(std::move(res_second[0].first));
	auto res = result_t();
	res.push_back(std::make_pair(std::move(res_first), std::move(res_second[0].second)));
	return std::move(res);
}