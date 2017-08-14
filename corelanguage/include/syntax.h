#pragma once
#include"lex.h"
#include<utility>
#include<vector>

std::vector<std::string> keywords{ "let","letrec","in","case","of","Pack" };

//
//template<typename type>
//class parser
//{
//public:
//	virtual std::vector<std::pair<type, std::vector<token>>>
//		operator()(const std::vector<token>& tokens) {throw std::exception();};
//};
//
//class pLit :public parser<std::string>
//{
//public:
//	std::vector<std::pair<std::string, std::vector<token>>>
//		operator()(const std::vector<token>& tokens) override;
//};