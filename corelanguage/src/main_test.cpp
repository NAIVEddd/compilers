#include<iostream>
#include"lex.h"

int main()
{
	std::string core{ "aa123\n >=<=nihao-- <= \n123 432" };
	auto res = lex(core);
	std::cout << res.size() << '\n';
	return 0;
}