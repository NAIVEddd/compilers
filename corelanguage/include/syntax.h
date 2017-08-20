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
	parser(parser&&) = delete;
	parser& operator=(parser&&) = delete;
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
		try
		{
			if (!prd(prog.at(0).name))
			{
				return result_t();
			}
			auto res = result_t();
			res.push_back(std::make_pair(prog.at(0).name, std::vector<token>(++prog.cbegin(), prog.cend())));
			return std::move(res);
		}
		catch (const std::out_of_range&)
		{
			return result_t();
		}
	}
private:
	std::function<bool(std::string)> prd;
};

template<typename T1, typename T2>
class pApply : public parser<T2>
{
public:
	using parser<T2>::result_t;

	pApply(parser<T1>& _p, std::function<T2(T1)> _f) : p(_p), trans(_f) {}
	pApply(parser<T1>&& _p, std::function<T2(T1)> _f) = delete;
	result_t
		operator()(std::vector<token>& prog)override
	{
		try
		{
			auto& tmp = p(prog);
			result_t res;
			res.push_back(std::make_pair(trans(tmp.at(0).first), tmp.at(0).second));
			return std::move(res);
		}
		catch (const std::out_of_range&)
		{
			return result_t();
		}
	}
private:
	parser<T1>& p;
	std::function<T2(T1)> trans;
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

	pNum() : 
		isNum([](std::string lhs) {return std::all_of(lhs.cbegin(), lhs.cend(), isdigit); }),
		toInt(isNum, [](std::string s) {return stoi(s); }) {};
	result_t
		operator()(std::vector<token>& prog)override
	{
		return std::move(toInt(prog));
	}

private:
	pSat<std::string> isNum;
	pApply<std::string, int> toInt;
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

template<typename T>
class pOr : public parser<T>
{
public:
	using parser<T>::result_t;

	pOr(parser<T>& _p1, parser<T>& _p2) : p1(_p1), p2(_p2) {}
	pOr(parser<T>&& _p1, parser<T>&& _p2) = delete;
	result_t
		operator()(std::vector<token>& prog)override
	{
		auto& res1 = p1(prog);
		auto& res2 = p2(prog);
		if (res1.size() == 0)
		{
			return std::move(res2);
		}
		else if (res2.size() == 0)
		{
			return std::move(res1);
		}
		for (auto& r : res2)
		{
			res1.push_back(r);
		}
		return std::move(res1);
	}
private:
	parser<T>& p1;
	parser<T>& p2;
};

template<typename T1, typename T2, typename T3>
class pThen : public parser<T3>
{
public:
	using parser<T3>::result_t;

	pThen(parser<T1>& _p1, parser<T2>& _p2, std::function<T3(T1, T2)> _f) :
		p1(_p1),
		p2(_p2),
		f(_f)
	{};
	result_t
		operator()(std::vector<token>& prog)override
	{
		try
		{
			auto& res1 = p1(prog);
			auto& res2 = p2(res1.at(0).second);
			result_t res;
			res.push_back(std::make_pair(f(res1[0].first, res2.at(0).first), res2.at(0).second));
			return std::move(res);
		}
		catch (const std::out_of_range&)
		{
			return result_t();
		}
	}

private:
	parser<T1>& p1;
	parser<T2>& p2;
	std::function<T3(T1, T2)> f;
};

template<typename T>
class pEmpty : parser<T>
{
public:
	using parser<T>::result_t;

	pEmpty() : set_default(false) {}
	pEmpty(T _val) : set_default(true), value(_val) {}
	result_t
		operator()(std::vector<token>& prog)override
	{
		if (set_default)
		{
			auto& res = result_t();
			res.push_back(std::make_pair(value, prog));
			return std::move(res);
		}
		return result_t();
	}

private:
	bool set_default;
	T value;
};

template<typename T1, typename T2, typename T3, typename T4>
class pThen3 : public parser<T4>
{
public:
	using parser<T4>::result_t;

	pThen3(parser<T1>& _p1, parser<T2>& _p2, parser<T3>& _p3, std::function<T4(T1, T2, T3)> _f) :
		p1(_p1),
		p2(_p2),
		p3(_p3),
		f(_f)
	{};
	pThen3(parser<T1>&& _p1, parser<T2>&& _p2, parser<T3>&& _p3, std::function<T4(T1, T2, T3)> _f) = delete;
	result_t
		operator()(std::vector<token>& prog)override
	{
		try
		{
			auto& res1 = p1(prog);
			auto& res2 = p2(res1.at(0).second);
			auto& res_second_3 = p3(res2.at(0).second);

			auto& res_first = f(res1[0].first, res2[0].first, res_second_3[0].first);

			auto res = result_t();
			res.push_back(std::make_pair(res_first, res_second_3[0].second));
			return std::move(res);
		}
		catch (const std::out_of_range&)
		{
			return result_t();
		}
	}

private:
	parser<T1>& p1;
	parser<T2>& p2;
	parser<T3>& p3;
	std::function<T4(T1, T2, T3)> f;
};

template<typename T1, typename T2, typename T3, typename T4, typename T5>
class pThen4 : parser<T5>
{
public:
	using parser<T5>::result_t;

	pThen4(parser<T1>& _p1, parser<T2>& _p2, parser<T3>& _p3, parser<T4>& _p4, std::function<T5(T1, T2, T3, T4)> _f) :
		p1(_p1),
		p2(_p2),
		p3(_p3),
		p4(_p4),
		f(_f)
	{};
	pThen4(parser<T1>&& _p1, parser<T2>&& _p2, parser<T3>&& _p3, parser<T4>&& _p4, std::function<T5(T1, T2, T3, T4)> _f) = delete;
	result_t
		operator()(std::vector<token>& prog)override
	{
		try
		{
			auto& res1 = p1(prog);
			auto& res2 = p2(res1.at(0).second);
			auto& res3 = p3(res2.at(0).second);
			auto& res_second_4 = p4(res3.at(0).second);

			auto& res_first = f(res1[0].first, res2[0].first, res3[0].first, res_second_4.at(0).first);

			auto res = result_t();
			res.push_back(std::make_pair(res_first, res_second_4[0].second));
			return std::move(res);
		}
		catch (const std::out_of_range&)
		{
			return result_t();
		}
	}

private:
	parser<T1>& p1;
	parser<T2>& p2;
	parser<T3>& p3;
	parser<T4>& p4;
	std::function<T5(T1, T2, T3, T4)> f;
};