#include<algorithm>
#include<cctype>
#include<set>
#include"base.h"
#include"lex.h"

std::set<std::string> twocharop{ std::string("=="),std::string("/="),std::string("<="),std::string(">="),std::string("->") };

std::vector<token> lex(std::string input)
{
	size_t col = 0, row = 0;
	std::vector<token> result;
	auto size = input.size();
	for (size_t i = 0; i != size; ++i)
	{
		unsigned char c = input[i];
		std::string name;
		if (c == '\n')
		{
			++row;
			col = 0;
			continue;
		}
		if (isspace(c))
		{
			++col;
			continue;
		}
		if (isalpha(c))
		{
			name.push_back(c);
			unsigned char c2 = '\0';
			while ((i + 1) != size)
			{
				c2 = input[i + 1];
				if (isalnum(c2))
				{
					name.push_back(c2);
					++i;
				}
				else
				{
					break;
				}
			}
			result.push_back(token(row, col, name));
			col += name.size() + 1;
			continue;
		}
		if (isdigit(c))
		{
			name.push_back(c);
			unsigned char c2 = '\0';
			while ((i + 1) != size)
			{
				c2 = input[i + 1];
				if (isalnum(c2))
				{
					name.push_back(c2);
					++i;
				}
				else
				{
					break;
				}
			}
			result.push_back(token(row, col, name));
			col += name.size() + 1;
			continue;
		}
		if (c == '-')
		{
			if ((i + 1) != size && input[i+1] == '-')
			{
				++i;
				while ((i + 1) != size && input[i+1] != '\n')
				{
					++i;
				}
				continue;
			}
		}
		if ((i + 1) != size)
		{
			std::string tmp;
			tmp.push_back(c);
			tmp.push_back(input[i + 1]);
			if (std::any_of(twocharop.begin(), twocharop.end(),
				[&tmp](const std::string& rhs)
			{
				return tmp == rhs;
			}))
			{
				result.push_back(token(row, col, tmp));
				col += 2;
				++i;
				continue;
			}
		}

		name.push_back(c);
		result.push_back(token(row, col, name));
		++col;
	}

	return result;
}