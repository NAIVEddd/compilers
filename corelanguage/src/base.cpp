#include"base.h"

token::token(size_t row_, size_t col_, std::string name_):
	row(row_),
	col(col_),
	name(std::move(name_))
{
}