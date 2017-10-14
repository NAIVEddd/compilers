#pragma once

#include"../expr.h"
#include"../syntax.h"

#include<string>
#include<vector>
#include<map>

class TiState;

std::string run(std::string& sourceCode);
TiState compile(std::shared_ptr<program>& prog);
std::vector<TiState> eval(TiState& initState);
std::string showResult(std::vector<TiState>& theResult);

using Addr = uint32_t;

struct Node
{
	~Node() = 0;
};

class TiState
{
public:
	using TiStack = std::vector<Addr>;
	using TiDump = std::vector<TiStack>;
	using TiGlobal = std::map<std::string, Addr>;

	class TiState::TiHeap
	{
	public:
		std::vector<Addr> addrs1;
		std::vector<Addr> addrs2;
		std::map<Addr, std::pair<std::shared_ptr<Node>, std::string>> programs;
		int allocTimes;
		int b;
		int c;

		TiHeap();
		TiHeap& Init();
		Addr Alloc(std::shared_ptr<ScDef>& node);
		Addr Alloc(NPrim& node);
		TiHeap& Update(Addr addr, std::shared_ptr<Node>& newNode);
		TiHeap& Free(Addr addr);
		std::shared_ptr<Node> LookUp(Addr addr);
		std::vector<Addr> Address() const;
		size_t Size() const;
		int GetAllocTimes() const;
		int GetFreeTimes() const;

	};
	class TiState::TiStats
	{
	public:

	};


	TiState();

	TiState& Init();
	TiStack& GetStack();
	TiDump& GetDump();
	TiGlobal& GetGlobal();
	TiHeap& GetHeap();
	TiStats& GetStats();

	TiStack m_stack;
	TiDump m_dump;
	TiGlobal m_global;
	TiHeap m_heap;
	TiStats m_stats;
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

enum class Primitive
{
	Neg,
	Add,
	Sub,
	Mul,
	Div,
};
class NPrim : public Node
{
public:
	NPrim(std::string name, Primitive prim)
		:name(std::move(name))
		,prim(prim)
	{}
	std::string name;
	Primitive prim;
};