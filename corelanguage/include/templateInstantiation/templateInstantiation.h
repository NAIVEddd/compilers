#pragma once

#include"../expr.h"
#include"../syntax.h"

#include<string>
#include<tuple>
#include<vector>

using Addr = uint32_t;
using TiStack = std::vector<Addr>;
using TiDump = std::vector<TiStack>;
class TiHeap;
using TiGlobal = std::pair<std::string, Addr>;
class TiStats;

using TiState = std::tuple<TiStack, TiDump, TiHeap, TiGlobal, TiStats>;

struct Node
{
	~Node() = 0;
};

class NAp : public Node
{
public:
	Addr left;
	Addr right;
};

template<typename T>
class NSC : public Node
{
public:
	std::string name;
	std::vector<T> params;
	expr body;
};

class NNum : public Node
{
public:
	int num;
};

class NInd : public Node
{
public:
	Addr addr;
};

enum class Primitive;
class NPrim : public Node
{
public:
	std::string name;
	Primitive prim;
};