#include"operatorExpr.h"
#include<numeric>

std::shared_ptr<expr>
combineOp_impl(std::shared_ptr<expr> coreExpr, std::shared_ptr<FoundOp> coreExpr1)
{
	auto op = std::make_shared<EVar>(coreExpr1->op);
	auto apLeft = std::make_shared<EAp>(std::move(op), std::move(coreExpr));
	return std::make_shared<EAp>(std::move(apLeft), std::move(coreExpr1->body));
}

std::shared_ptr<expr>
combineOp_impl(std::shared_ptr<expr> coreExpr, std::shared_ptr<NoOp> noOp)
{
	return std::move(coreExpr);
}

std::shared_ptr<expr>
combineOp(std::shared_ptr<expr> coreExpr, std::shared_ptr<partExpr> coreExpr1)
{
	if (coreExpr1->type == 1)
	{
		return combineOp_impl(std::move(coreExpr), std::move(std::dynamic_pointer_cast<NoOp>(coreExpr1)));
	}
	else if (coreExpr1->type == 2)
	{
		return combineOp_impl(std::move(coreExpr), std::move(std::dynamic_pointer_cast<FoundOp>(coreExpr1)));
	}
	throw std::exception("not valid PartExpr.");
}

pExpr_p1::result_t 
pExpr_p1::operator()(std::vector<token>& prog)
{
	pExpr_p2 p2;
	pExpr_p1r p1r;
	pThen<std::shared_ptr<expr>,std::shared_ptr<partExpr>,std::shared_ptr<expr>>
		execParse(p2, p1r, combineOp);
	return execParse(prog);
}

pExpr_p2::result_t 
pExpr_p2::operator()(std::vector<token>& prog)
{
	pExpr_p3 p3;
	pExpr_p2r p2r;
	pThen<std::shared_ptr<expr>, std::shared_ptr<partExpr>, std::shared_ptr<expr>>
		execParse(p3, p2r, combineOp);
	return execParse(prog);
}

pExpr_p3::result_t 
pExpr_p3::operator()(std::vector<token>& prog)
{
	pExpr_p4 p4;
	pExpr_p3r p3r;
	pThen<std::shared_ptr<expr>, std::shared_ptr<partExpr>, std::shared_ptr<expr>>
		execParse(p4, p3r, combineOp);
	return execParse(prog);
}

pExpr_p4::result_t 
pExpr_p4::operator()(std::vector<token>& prog)
{
	pExpr_p5 p5;
	pExpr_p4r p4r;
	pThen<std::shared_ptr<expr>, std::shared_ptr<partExpr>, std::shared_ptr<expr>>
		execParse(p5, p4r, combineOp);
	return execParse(prog);
}

pExpr_p5::result_t 
pExpr_p5::operator()(std::vector<token>& prog)
{
	pExpr_p6 p6;
	pExpr_p5r p5r;
	pThen<std::shared_ptr<expr>, std::shared_ptr<partExpr>, std::shared_ptr<expr>>
		execParse(p6, p5r, combineOp);
	return execParse(prog);
}

pExpr_p6::result_t 
pExpr_p6::operator()(std::vector<token>& prog)
{
	pAExpr paexpr;
	pOneOrMore<std::shared_ptr<expr>> getAExprs(paexpr);
	pApply<std::vector<std::shared_ptr<expr>>, std::shared_ptr<expr>>
		app_chain(getAExprs, [](std::vector<std::shared_ptr<expr>> apps)->std::shared_ptr<expr>
		{
			if (apps.size() == 0)
			{
				throw std::exception("there is no expr to construct EAps.");
			}
			return std::accumulate(++apps.begin(), apps.end(), apps.at(0), [](std::shared_ptr<expr> left, std::shared_ptr<expr> right)
				{
					return std::make_shared<EAp>(std::move(left), std::move(right));
				});
		});
	return app_chain(prog);
}

pExpr_p1r::result_t 
pExpr_p1r::operator()(std::vector<token>& prog)
{
	pLit<std::string> opSb("|");
	pExpr_p1 p1;
	pThen<std::string,std::shared_ptr<expr>,std::shared_ptr<partExpr>>
		getPartExpr(opSb, p1, [](std::string op, std::shared_ptr<expr> body)
			{
				return std::make_shared<FoundOp>(op, body);
			});

	auto noOp = std::make_shared<NoOp>();
	pEmpty<std::shared_ptr<partExpr>> getNoOp(noOp);

	pOr<std::shared_ptr<partExpr>>
		execParse(getPartExpr, getNoOp);
	return execParse(prog);
}

pExpr_p2r::result_t 
pExpr_p2r::operator()(std::vector<token>& prog)
{
	pLit<std::string> opSb("&");
	pExpr_p2 p2;
	pThen<std::string, std::shared_ptr<expr>, std::shared_ptr<partExpr>>
		getPartExpr(opSb, p2, [](std::string op, std::shared_ptr<expr> body)
	{
		return std::make_shared<FoundOp>(op, body);
	});

	auto noOp = std::make_shared<NoOp>();
	pEmpty<std::shared_ptr<partExpr>> getNoOp(noOp);

	pOr<std::shared_ptr<partExpr>>
		execParse(getPartExpr, getNoOp);
	return execParse(prog);
}

pExpr_p3r::result_t 
pExpr_p3r::operator()(std::vector<token>& prog)
{
	auto& ref = operators[4];
	pSat<std::string> opSb([&ref](std::string input)
		{
			return std::find(ref.begin(),ref.end(),input) != ref.end();
		});
	pExpr_p4 p4;
	pThen<std::string, std::shared_ptr<expr>, std::shared_ptr<partExpr>>
		getPartExpr(opSb, p4, [](std::string op, std::shared_ptr<expr> body)
	{
		return std::make_shared<FoundOp>(op, body);
	});

	auto noOp = std::make_shared<NoOp>();
	pEmpty<std::shared_ptr<partExpr>> getNoOp(noOp);

	pOr<std::shared_ptr<partExpr>>
		execParse(getPartExpr, getNoOp);
	return execParse(prog);
}

pExpr_p4r::result_t 
pExpr_p4r::operator()(std::vector<token>& prog)
{
	pLit<std::string> opSbPlus("+");
	pExpr_p4 p4;
	pThen<std::string, std::shared_ptr<expr>, std::shared_ptr<partExpr>>
		getPartExpr1(opSbPlus, p4, [](std::string op, std::shared_ptr<expr> body)
	{
		return std::make_shared<FoundOp>(op, body);
	});

	pLit<std::string> opSbMinus("-");
	pExpr_p5 p5;
	pThen<std::string, std::shared_ptr<expr>, std::shared_ptr<partExpr>>
		getPartExpr2(opSbMinus, p5, [](std::string op, std::shared_ptr<expr> body)
	{
		return std::make_shared<FoundOp>(op, body);
	});

	auto noOp = std::make_shared<NoOp>();
	pEmpty<std::shared_ptr<partExpr>> getNoOp(noOp);

	pOr<std::shared_ptr<partExpr>>
		execParseTmp(getPartExpr1, getPartExpr2);

	pOr<std::shared_ptr<partExpr>>
		execParse(execParseTmp, getNoOp);
	return execParse(prog);
}

pExpr_p5r::result_t 
pExpr_p5r::operator()(std::vector<token>& prog)
{
	pLit<std::string> opSbMulty("*");
	pExpr_p5 p5;
	pThen<std::string, std::shared_ptr<expr>, std::shared_ptr<partExpr>>
		getPartExpr1(opSbMulty, p5, [](std::string op, std::shared_ptr<expr> body)
	{
		return std::make_shared<FoundOp>(op, body);
	});

	pLit<std::string> opSbDevide("/");
	pExpr_p6 p6;
	pThen<std::string, std::shared_ptr<expr>, std::shared_ptr<partExpr>>
		getPartExpr2(opSbDevide, p6, [](std::string op, std::shared_ptr<expr> body)
	{
		return std::make_shared<FoundOp>(op, body);
	});

	auto noOp = std::make_shared<NoOp>();
	pEmpty<std::shared_ptr<partExpr>> getNoOp(noOp);

	pOr<std::shared_ptr<partExpr>>
		execParseTmp(getPartExpr1, getPartExpr2);

	pOr<std::shared_ptr<partExpr>>
		execParse(execParseTmp, getNoOp);
	return execParse(prog);
}
