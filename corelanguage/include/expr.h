#pragma once
#include"lex.h"
#include<vector>
#include<memory>

class TiState;
using Addr = uint32_t;
class expr
{
public:
	virtual ~expr() = default;
	virtual void Instantiate(TiState& state) = 0;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) = 0;
};

class EVar : public expr
{
public:
	EVar(std::string name) :
		name(std::move(name))
	{}
	~EVar() override {}
	virtual void Instantiate(TiState& state) override;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) override;

private:
	std::string name;
};

class ENum : public expr
{
public:
	ENum(int num) :
		num(num)
	{}
	~ENum() override {}
	virtual void Instantiate(TiState& state) override;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) override;

private:
	int num;
};

class EConstr : public expr
{
public:
	EConstr(int tag, int arity) :
		tag(tag),
		arity(arity)
	{}
	~EConstr() override {}
	virtual void Instantiate(TiState& state) override;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) override;

private:
	int tag, arity;
};

class EAp : public expr
{
public:
	EAp(std::shared_ptr<expr> left, std::shared_ptr<expr> right) :
		left(std::move(left)),
		right(std::move(right))
	{}
	~EAp() override {}
	virtual void Instantiate(TiState& state) override;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) override;

private:
	std::shared_ptr<expr> left;
	std::shared_ptr<expr> right;
};

class ELet : public expr
{
public:
	ELet(bool isRec, std::vector<std::pair<std::string, std::shared_ptr<expr>>> defines, std::shared_ptr<expr> exprs) :
		isRec(isRec),
		defines(std::move(defines)),
		exprs(std::move(exprs))
	{}
	~ELet() override {}
	virtual void Instantiate(TiState& state) override;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) override;

private:
	bool isRec;
	std::vector<std::pair<std::string, std::shared_ptr<expr>>> defines;
	std::shared_ptr<expr> exprs;
};

class EAlter
{
public:
	EAlter(int tag_, std::vector<std::string>&& params_, std::shared_ptr<expr> right_) :
		tag(tag_),
		params(std::move(params_)),
		right(right_)
	{}
	~EAlter() = default;
	int tag;
	std::vector<std::string> params;
	std::shared_ptr<expr> right;
};

class ECase : public expr
{
public:
	ECase(std::shared_ptr<expr> exprs, std::vector<std::shared_ptr<EAlter>> alters) :
		exprs(std::move(exprs)),
		alters(std::move(alters))
	{}
	~ECase() override {}
	virtual void Instantiate(TiState& state) override;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) override;

private:
	std::shared_ptr<expr> exprs;
	std::vector<std::shared_ptr<EAlter>> alters;
};

class ELam : public expr
{
public:
	ELam(std::vector<std::string> vars, std::shared_ptr<expr>& body) :
		params(std::move(vars)),
		body(body)
	{}
	~ELam() override {}
	virtual void Instantiate(TiState& state) override;
	virtual void InstantiateAndUpdate(TiState& state, Addr addr) override;

private:
	std::vector<std::string> params;
	std::shared_ptr<expr> body;
};

class ScDef
{
public:
	ScDef(std::string name, std::vector<std::string> params, std::shared_ptr<expr> body) :
		name(std::move(name)),
		params(std::move(params)),
		body(std::move(body))
	{}
	std::string name;
	std::vector<std::string> params;
	std::shared_ptr<expr> body;
};

class program
{
public:
	program(std::vector<std::shared_ptr<ScDef>> defs) :
		defs(std::move(defs))
	{}
	std::vector<std::shared_ptr<ScDef>> defs;
};