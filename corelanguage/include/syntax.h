#pragma once
#include"lex.h"
#include"expr.h"
#include<utility>
#include<vector>
#include<algorithm>
#include<functional>

static std::vector<std::string> keywords{ "let","letrec","in","case","of","Pack" };
static std::vector<std::vector<std::string>> operators{ std::vector<std::string>{ "*" }, 
														std::vector<std::string>{ "/" },
														std::vector<std::string>{ "+" },
														std::vector<std::string>{ "-" },
														std::vector<std::string>{ "==","/=",">",">=","<","<="},
														std::vector<std::string>{ "&" },
														std::vector<std::string>{ "|" } };

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

	pVar() : isVar([](std::string lhs) {return std::find(keywords.cbegin(), keywords.cend(), lhs) == keywords.cend() && lhs.size() > 0 && isalpha(lhs[0]); }) {};
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
class pEmpty : public parser<T>
{
public:
	using parser<T>::result_t;

	pEmpty() : set_default(false) {}
	pEmpty(T _val) : set_default(true), value(_val) {}
	result_t
		operator()(std::vector<token>& prog)override
	{
		auto& res = result_t();
		if (set_default)
		{
			res.push_back(std::make_pair(value, prog));
		}
		else
		{
			res.push_back(std::make_pair(T(), prog));
		}
		return std::move(res);
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

			auto res_first = f(res1[0].first, res2[0].first, res_second_3.at(0).first);

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

template<typename T> class pOneOrMore;
template<typename T>
class pZeroOrMore : public parser<std::vector<T>>
{
public:
	using parser<std::vector<T>>::result_t;

	pZeroOrMore(parser<T>& _p):
		p(_p)
	{}
	result_t
		operator()(std::vector<token>& prog)override
	{
		pEmpty<std::vector<T>> empty;
		pOr<std::vector<T>> parse_zero_or_more(p, empty);
		return std::move(parse_zero_or_more(prog));
	}
private:
	pOneOrMore<T> p;
};

template<typename T>
class pOneOrMore : public parser<std::vector<T>>
{
public:
	using parser<std::vector<T>>::result_t;

	pOneOrMore(parser<T>& _p) :
		p(_p)
	{};
	result_t
		operator()(std::vector<token>& prog)override
	{
		if (prog.size() == 0)
		{
			return result_t();
		}
		try
		{
			auto res_first = p(prog);
			auto res_second = this->operator()(res_first.at(0).second);
			auto res = result_t();
			auto tmp = result_t::value_type::first_type();
			tmp.push_back(res_first[0].first);
			if (res_second.size() == 0)
			{
				res.push_back(std::make_pair(tmp, result_t::value_type::second_type()));
				return std::move(res);
			}
			res.push_back(std::make_pair(tmp, res_second[0].second));
			for (auto& r : res_second[0].first)
			{
				res[0].first.push_back(r);
			}
			return std::move(res);
		}
		catch (const std::out_of_range&)
		{
			auto res = result_t();
			res.push_back(
				std::make_pair(result_t::value_type::first_type(),
				prog));
			return std::move(res);
		}
	}
private:
	parser<T>& p;
};

template<typename T1, typename T2>
class pOneOrMoreWithSpt : public parser<std::vector<T1>>
{
public:
	using parser<std::vector<T1>>::result_t;

	pOneOrMoreWithSpt(parser<T1>& p1, parser<T2>& p2):
		pct(p1),
		pspt(p2)
	{}
	result_t 
		operator()(std::vector<token>& prog)override
	{
		result_t res;
		try
		{
			auto& res_first = pct(prog);

			{
				result_t::value_type::first_type tmp;
				tmp.push_back(res_first.at(0).first);
				res.push_back(std::make_pair(tmp,res_first[0].second));
			}

			pThen<T2, T1, T1> parse_more(pspt, pct, [](T2 a, T1 b) {return b; });
			pZeroOrMore<T1> exec_parse(parse_more);
			auto& res_second = exec_parse(res_first[0].second);

			for (auto& r : res_second.at(0).first)
			{
				res[0].first.push_back(r);
			}
			res[0].second = res_second[0].second;
			return std::move(res);
		}
		catch (const std::out_of_range&)
		{
			return std::move(res);
		}
	}

private:
	parser<T1>& pct;
	parser<T2>& pspt;
};


class pExpr;

class pAlts : public parser<std::vector<std::shared_ptr<EAlter>>>
{
public:
	using parser<std::vector<std::shared_ptr<EAlter>>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override
	{
		pAlt alt;
		pLit<std::string> sliceSym(";");
		pOneOrMoreWithSpt<std::shared_ptr<EAlter>, std::string> getAlts(alt, sliceSym);
		return getAlts(prog);
	}

private:
	class pAlt : public parser<std::shared_ptr<EAlter>>
	{
	public:
		using parser<std::shared_ptr<EAlter>>::result_t;
		result_t
			operator()(std::vector<token>& prog)override;
	};
};

class pDefs : public parser<std::vector<std::pair<std::string, std::shared_ptr<expr>>>>
{
public:
	using parser<std::vector<std::pair<std::string, std::shared_ptr<expr>>>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override
	{
		pLit<std::string> slipeSym(";");
		pDef def;
		pOneOrMoreWithSpt<std::pair<std::string, std::shared_ptr<expr>>, std::string>
			getDefs(def, slipeSym);
		return getDefs(prog);
	}
	
private:
	class pDef : public parser<std::pair<std::string, std::shared_ptr<expr>>>
	{
	public:
		using parser<std::pair<std::string, std::shared_ptr<expr>>>::result_t;
		result_t
			operator()(std::vector<token>& prog)override;
	};
};

class pLambda : public parser<std::shared_ptr<ELam>>
{
public:
	using parser<std::shared_ptr<ELam>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override;
};

class pCase : public parser<std::shared_ptr<ECase>>
{
public:
	using parser<std::shared_ptr<ECase>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override;
};

class pLet : public parser<std::shared_ptr<ELet>>
{
public:
	using parser<std::shared_ptr<ELet>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override;
};

class pAExpr : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override;
};

class pConstr : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override;
};

class pExpr : public parser<std::shared_ptr<expr>>
{
public:
	using parser<std::shared_ptr<expr>>::result_t;
	result_t
		operator()(std::vector<token>& prog)override
	{
		return result_t();
	}
};

class pScDef : public parser<std::shared_ptr<ScDef>>
{
public:
	using parser<std::shared_ptr<ScDef>>::result_t;
};

class pProgram : public parser<program>
{
public:
	using parser<program>::result_t;

};