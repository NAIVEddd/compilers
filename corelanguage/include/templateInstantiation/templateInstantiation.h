#pragma once

#include"../expr.h"
#include"../syntax.h"

#include<string>
#include<vector>
#include<map>
#include<list>

class TiState;
class NPrim;

std::string run(std::string& sourceCode);
TiState compile(std::shared_ptr<program>& prog);
std::vector<TiState> eval(TiState& initState);
std::string showResult(std::vector<TiState>& theResult);

using Addr = uint32_t;

struct Node
{
	virtual ~Node() = 0;
	virtual TiState& Advance(TiState& state) = 0;
};

class TiState
{
public:
	using TiStack = std::list<Addr>;
	using TiDump = std::vector<TiStack>;
	using TiGlobal = std::map<std::string, Addr>;

	class TiHeap
	{
	public:
		TiHeap();
		TiHeap& Init();		// obtain the 'main' function's addr
		Addr Alloc(std::shared_ptr<ScDef>& node);
		Addr Alloc(NPrim& node);
		TiHeap& Update(Addr addr, std::shared_ptr<Node>& newNode);
		TiHeap& Free(Addr addr);
		std::shared_ptr<Node> LookUp(Addr addr);
		const std::list<Addr>& Address() const;
		size_t Size() const;
		int GetAllocTimes() const;
		int GetUpdateTimes() const;
		int GetFreeTimes() const;

	private:
		std::vector<Addr> addrs1;
		std::list<Addr> addrs2;
		std::map<Addr, std::pair<std::shared_ptr<Node>, std::string>> programs;
		int allocTimes;
		int updateTimes;
		int freeTimes;
	};
	class TiStats
	{
	public:
		int i = 0;
	};


	TiState();

	TiState& Init();
	TiStack& GetStack();
	TiDump& GetDump();
	TiGlobal& GetGlobal();
	TiHeap& GetHeap();
	TiStats& GetStats();

private:
	TiStack m_stack;
	TiDump m_dump;
	TiGlobal m_global;
	TiHeap m_heap;
	TiStats m_stats;
};

class NAp : public Node
{
public:
	NAp(Addr l, Addr r) :left(l), right(r) {}
	TiState& Advance(TiState& state) override;

	Addr left;
	Addr right;
};

class NSC : public Node
{
public:
	NSC(std::string name, std::vector<std::string> params, std::shared_ptr<expr>& body)
		: name(std::move(name))
		, params(std::move(params))
		, body(body)
	{}
	TiState& Advance(TiState& state) override;

	std::string name;
	std::vector<std::string> params;
	std::shared_ptr<expr> body;
};

class NNum : public Node
{
public:
	NNum(int num) :num(num) {}
	TiState& Advance(TiState& state) override;

	int num;
};

class NInd : public Node
{
public:
	NInd(Addr addr) :addr(addr) {}
	TiState& Advance(TiState& state) override;

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
		: name(std::move(name))
		, prim(prim)
	{}
	TiState& Advance(TiState& state) override;

	std::string name;
	Primitive prim;
};