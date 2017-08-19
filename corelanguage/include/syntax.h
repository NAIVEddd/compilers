#pragma once
#include"lex.h"
#include<utility>
#include<vector>
#include<algorithm>
#include<functional>

std::vector<std::string> keywords{ "let","letrec","in","case","of","Pack" };

template<typename T>
class parser
{
public:
	using result_t = std::vector<std::pair<T, std::vector<token>>>;

	parser() {};
	virtual ~parser() {};

	virtual result_t
		operator()(std::vector<token>& prog) { throw std::exception(); };
};

template<typename T>
class pSat : public parser<T>
{
public:
	using parser<T>::result_t;

	pSat(std::function<bool(std::string)> _prd) : prd(_prd) {};
	parser::result_t
		operator()(std::vector<token>& prog)
	{
		if (prog.size() == 0 || !prd(prog.at(0).name))
		{
			return result_t();
		}
		auto res = result_t();
		res.push_back(std::make_pair(prog.at(0).name, std::vector<token>(++prog.cbegin(), prog.cend())));
		return std::move(res);
	}
private:
	std::function<bool(std::string)> prd;
};

template<typename T>
class pLit :public parser<T> {};

template<>
class pLit<std::string> : public parser<std::string> 
{
public:
	using parser<std::string>::result_t;
	
	pLit(std::string tag) :prd([tag](std::string lhs) {return lhs == tag; }) {};
	~pLit()override {};
	result_t
		operator()(std::vector<token>& prog)override
	{
		return std::move(prd(prog));
	};
private:
	pSat<std::string> prd;
};

class pNum :public parser<int>
{
public:
	using parser<int>::result_t;

	pNum() : isNum([](std::string lhs) {return std::all_of(lhs.cbegin(), lhs.cend(), isdigit); }){};
	result_t
		operator()(std::vector<token>& prog)override
	{
		auto tmp = isNum(prog);
		if (tmp.size() == 0)
		{
			return result_t();
		}
		auto res = result_t();
		res.push_back(std::make_pair(std::stoi(prog.at(0).name), std::vector<token>(std::move(tmp.at(0).second))));
		return std::move(res);
	}

private:
	pSat<std::string> isNum;
};

class pVar : public parser<std::string>
{
public:
	using parser<std::string>::result_t;

	pVar() : isVar([](std::string lhs) {return std::find(keywords.cbegin(), keywords.cend(), lhs) == keywords.cend(); }) {};
	result_t 
		operator()(std::vector<token>& prog)override
	{
		return std::move(isVar(prog));
	}
private:
	pSat<std::string> isVar;
};