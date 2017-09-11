#pragma once
#include"expr.h"
#include"syntax.h"

class partExpr
{
public:
	virtual ~partExpr() = default;

	uint32_t type;
};

class NoOp : public partExpr
{
public:
	NoOp() { type = 1; }
private:
};

class FoundOp : public partExpr
{
public:
	FoundOp(std::string op, std::shared_ptr<expr> coreExpr):
		op(std::move(op)),
		body(std::move(coreExpr))
	{type = 2;}

	std::string op;
	std::shared_ptr<expr> body;
};

std::shared_ptr<expr>
combineOp_impl(std::shared_ptr<expr> coreExpr, std::shared_ptr<FoundOp> coreExpr1);

std::shared_ptr<expr>
combineOp_impl(std::shared_ptr<expr> coreExpr, std::shared_ptr<NoOp> noOp);

std::shared_ptr<expr>
combineOp(std::shared_ptr<expr> coreExpr, std::shared_ptr<partExpr> coreExpr1);

// construct expr from partExpr

class pExpr_p1 : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;

	pExpr_p1() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p2 : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;

	pExpr_p2() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p3 : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;

	pExpr_p3() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p4 : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;

	pExpr_p4() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p5 : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;

	pExpr_p5() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p6 : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;

	pExpr_p6() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

// parser the partExpr.

class pExpr_p1r : public parser<std::shared_ptr<partExpr>>
{
public:
	using parser<std::shared_ptr<partExpr>>::result_t;

	pExpr_p1r() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p2r : public parser<std::shared_ptr<partExpr>>
{
public:
	using parser<std::shared_ptr<partExpr>>::result_t;

	pExpr_p2r() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p3r : public parser<std::shared_ptr<partExpr>>
{
public:
	using parser<std::shared_ptr<partExpr>>::result_t;

	pExpr_p3r() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p4r : public parser<std::shared_ptr<partExpr>>
{
public:
	using parser<std::shared_ptr<partExpr>>::result_t;

	pExpr_p4r() {}
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr_p5r : public parser<std::shared_ptr<partExpr>>
{
public:
	using parser<std::shared_ptr<partExpr>>::result_t;

	pExpr_p5r() {}
	result_t
		operator()(std::vector<token>& prog)override;
};