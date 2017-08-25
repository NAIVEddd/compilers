#pragma once
#include"lex.h"
#include<vector>
#include<memory>

class expr
{
public:
	virtual ~expr() = 0;
};

class EVar : public expr
{
public:
	~EVar() override {}

private:
	std::string name;
};

class ENum : public expr
{
public:
	~ENum() override {}

private:
	int num;
};

class EConstr : public expr
{
public:
	~EConstr() override {}

private:
	int num1, num2;
};

class EAp : public expr
{
public:
	~EAp() override {}

private:
	std::shared_ptr<expr> left;
	std::shared_ptr<expr> right;
};

class ELet : public expr
{
public:
	~ELet() override {}

private:
	bool isRec;
	std::vector<std::shared_ptr<expr>> init_;
	std::shared_ptr<expr> exec;
};

class EAlter
{
public:
	EAlter(int tag_, std::vector<std::string>&& params_, std::shared_ptr<expr> right_):
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
	~ECase() override {}

private:
	std::shared_ptr<expr> match1;
	EAlter& other;
};

class ELam : public expr
{
public:
	~ELam() override {}

private:
	std::vector<std::string> params;
	std::shared_ptr<expr> body;
};

class ScDef
{
public:
	std::string name;
	std::vector<std::string> params;
	std::shared_ptr<expr> body;
};

class program
{
public:
	std::vector<ScDef> defs;
};