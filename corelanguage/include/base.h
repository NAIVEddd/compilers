#pragma once
#include<string>

class token
{
public:
	token(size_t row_, size_t col_, std::string name_);
	~token() = default;
	token(const token& rhs) = default;
	token& operator=(const token& rhs) = default;
private:
	size_t row,col;
	std::string name;
};