#pragma once
#include"util.h"
#include<fstream>
#include<vector>

enum class token_type
{
	unknown,
	identifier,
	num,
	keyword,
};

struct token
{
	using token_list = std::vector<token>;
	using name_type = string_t;
	void set_name(name_type _name);
	token_type type;
	string_t name;
	size_t line;
	size_t col;
};

token::token_list lexer(istream_t& is);