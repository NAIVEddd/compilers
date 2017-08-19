#pragma once
#include"lex.h"
#include"expr.h"
#include<string>
#include<vector>
#include<utility>
#include<iostream>

template<typename T>
class parser
{
public:
	parser() {};
	virtual ~parser() {};

	virtual std::vector<std::pair<T, std::vector<std::string>>>
		operator()(std::vector<token>& prog) { return std::vector<std::pair<T, std::vector<std::string>>>(); };
};

template<typename T>
class PLit : public parser<T> {};

template<>
class PLit<std::string> : public parser<std::string>
{
public:
	PLit() {};
	~PLit()override {};
	std::vector<std::pair<std::string, std::vector<std::string>>>
		operator()(std::vector<token>& prog)override {
		std::cout << "hello\n"; return std::vector<std::pair<std::string, std::vector<std::string>>>
			();
	};
};
expr parse(std::vector<token>& programs);