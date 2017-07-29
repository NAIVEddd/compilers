#include"token.h"
#include<sstream>
#include<set>

std::set<token::name_type> keyword;
void token::set_name(name_type _name)
{
	name = _name;
	if (keyword.find(_name) != keyword.end())
	{
		type = token_type::keyword;
	}
	else
	{
		type = token_type::identifier;
	}
}

token::token_list lexer(istream_t & is)
{
	ifstream_t::char_type c{};
	token::name_type temp;
	token::token_list result;
	size_t line = 0, col = 0;

	token token_temp;
	token_temp.col = col;
	token_temp.line = line;
	while (is.get(c))
	{
		if (c != '\r' && c != '\n' && c != ' ' && c != '\t')
		{
			temp.push_back(c);
		}
		else
		{
			if (temp.size() == 0)
			{
				continue;
			}
			token_temp.set_name(temp);
			result.push_back(token_temp);
			temp.clear();
			
			if (c == '\n')
			{
				++line;
				col = 0;
			}
			token_temp.col = col;
			token_temp.line = line;
		}
		++col;
	}

	token_temp.set_name(temp);
	result.push_back(token_temp);
	return result;
}