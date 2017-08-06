#include"base.h"
#include"syntax.h"
#include<algorithm>
#include<functional>
using prd = std::function<bool(token)>;
using tokens_list = std::vector<std::pair<std::string, std::vector<token>>>;

tokens_list
pSat(const std::vector<token>& tokens, prd func)
{
	tokens_list result;
	if (tokens.size() == 0)
	{
		return result;
	}
	if (func(tokens[0]))
	{
		result.push_back(std::pair<std::string, std::vector<token>>{ tokens[0].name, std::vector<token>(++tokens.begin(), tokens.end()) });
		return result;
	}
	else
	{
		return result;
	}
}

tokens_list
pLit(const std::vector<token>& tokens, std::string name)
{
	return pSat(tokens, [&name](token tok)->bool {return name == tok.name; });
}

using patten = std::function<tokens_list(const std::vector<token>&, prd)>;
template<typename out, typename in>
class transFunc
{
public:
	transFunc(std::function<out(in)> func_) :func(func_) {};
	out operator()(in para) { return func(para); }

	std::function<out(in)> func;
};

template<typename out, typename in>
std::vector<std::pair<out, std::vector<token>>>
pApply(const std::vector<token>& tokens, patten p, prd prdFunc, transFunc<out,in> trans)
{
	std::vector<std::pair<out, std::vector<token>>> result;
	auto list = p(tokens, prdFunc);
	if (list.size() != 0)
	{
		auto newfst = trans(list[0].first);
		result.push_back({ newfst,list[0].second });
	}
	return result;
}

std::vector<std::pair<std::string, std::vector<token>>>
pVar(const std::vector<token>& tokens)
{
	const auto & keywordList = keywords;
	prd isVar = [& keywordList](const token& tok) { return std::find(keywordList.cbegin(), keywordList.cend(), tok.name) == keywordList.cend(); };
	return pSat(tokens, isVar);
}

std::vector<std::pair<uint32_t, std::vector<token>>>
pNum(const std::vector<token>& tokens)
{
	prd isNum = [](const token& tok) { return std::all_of(tok.name.begin(), tok.name.end(), isdigit); };
	transFunc<uint32_t, std::string> toInt([](std::string str) { return (uint32_t)std::stol(str); });
	return pApply(tokens,
		pSat,
		isNum,
		toInt);
}