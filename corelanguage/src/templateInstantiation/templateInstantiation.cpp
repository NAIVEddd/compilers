#include"templateInstantiation/templateInstantiation.h"
#include<map>
#include<memory>
#include<functional>

std::vector<NPrim>&
getPrDefs()
{
	static std::vector<NPrim> prDefs;
	if (prDefs.size() == 0)
	{
		prDefs.push_back(NPrim("negate", Primitive::Neg));
		prDefs.push_back(NPrim("+", Primitive::Add));
		prDefs.push_back(NPrim("-", Primitive::Sub));
		prDefs.push_back(NPrim("*", Primitive::Mul));
		prDefs.push_back(NPrim("/", Primitive::Div));
	}
	return prDefs;
}

std::pair<TiState::TiHeap, TiState::TiGlobal>
mapAccuml(std::function<std::pair<TiState::TiHeap, TiState::TiGlobal>(TiState::TiHeap, std::shared_ptr<ScDef>&)> f,
	TiState::TiHeap& initHeap,
	std::shared_ptr<program>& prog)
{
	TiState::TiGlobal res_global;
	TiState::TiHeap res_heap = initHeap;
	for (auto& def : prog->defs)
	{
		auto res = f(res_heap, def);
		res_global.insert(res.second.begin(), res.second.end());
		res_heap = std::move(res.first);
	}
	return std::make_pair(res_heap, res_global);
}

std::pair<TiState::TiHeap, TiState::TiGlobal>
mapAccuml(std::function<std::pair<TiState::TiHeap, TiState::TiGlobal>(TiState::TiHeap, NPrim&)> f,
	TiState::TiHeap& initHeap,
	std::vector<NPrim>& prog)
{
	TiState::TiGlobal res_global;
	TiState::TiHeap res_heap = initHeap;
	for (auto& def : prog)
	{
		auto res = f(res_heap, def);
		res_global.insert(res.second.begin(), res.second.end());
		res_heap = std::move(res.first);
	}
	return std::make_pair(res_heap, res_global);
}

std::pair<TiState::TiHeap, TiState::TiGlobal>
buildInitialHeap(std::shared_ptr<program>& prog)
{
	auto allocateSc = [](TiState::TiHeap heap, std::shared_ptr<ScDef>& def)
	{
		TiState::TiGlobal res_global;
		auto newAddr = heap.Alloc(def);
		res_global.insert(std::make_pair(def->name, newAddr));
		return std::move(std::make_pair(heap, res_global));
	};
	auto allocatePr = [](TiState::TiHeap heap, NPrim& def)
	{
		TiState::TiGlobal res_global;
		auto newAddr = heap.Alloc(def);
		res_global.insert(std::make_pair(def.name, newAddr));
		return std::move(std::make_pair(heap, res_global));
	};

	TiState::TiHeap heap;
	auto tmp_res = mapAccuml(allocateSc, heap, prog);
	auto tmp_res2 = mapAccuml(allocatePr, tmp_res.first, getPrDefs());
	for (auto& kv : tmp_res2.second)
	{
		tmp_res.second.insert(kv);
	}
	return std::move(std::make_pair(tmp_res2.first, tmp_res.second));
}

std::string
run(std::string & sourceCode)
{
	return std::string();
}

TiState
compile(std::shared_ptr<program> & prog)
{
	TiState res;
	auto heap_global = buildInitialHeap(prog);
	res.GetHeap() = std::move(heap_global.first);
	res.GetGlobal() = std::move(heap_global.second);
	return res.Init();
}

bool
isDataNode(std::shared_ptr<Node>& node)
{
	try
	{
		std::dynamic_pointer_cast<NNum>(node);
		return true;
	}
	catch (...)
	{
		return false;
	}
}

bool
tiFinalState(TiState& state)
{
	if (state.GetStack().size() == 0)
	{
		throw "empty stack.";
	}
	auto addr = state.GetStack().front();
	if (state.GetDump().size() == 0 && isDataNode(state.GetHeap().LookUp(addr)))
	{
		return true;
	}
	return false;
}

// do the administration work between steps
//TiState&
//doAdmin(TiState& state)
//{
//	return state;
//}

TiState
advanceState(TiState& state)
{
	auto addr = state.GetStack().front();
	return state.GetHeap().LookUp(addr)->Advance(state);
}

std::vector<TiState>
eval(TiState & initState)
{
	std::vector<TiState> result;
	result.push_back(initState);
	if (!tiFinalState(initState))
	{
		advanceState(initState);
		//doAdmin(initState);
		auto tmp = eval(initState);
		for (auto& t : tmp)
		{
			result.push_back(std::move(t));
		}
	}
	return std::move(result);
}

std::string
showResult(std::vector<TiState>& theResult)
{
	return std::string();
}

TiState::TiHeap::TiHeap()
	: allocTimes(0)
	, updateTimes(0)
	, freeTimes(0)
{
	addrs1.push_back(1);
}

Addr
TiState::TiHeap::Alloc(std::shared_ptr<ScDef>& node)
{
	auto addr = addrs1[0]++;
	addrs2.push_back(addr);
	programs.insert(std::make_pair(addr, std::make_pair(std::make_shared<NSC>(node->name, node->params, node->body), node->name)));
	++allocTimes;
	return addr;
}

Addr
TiState::TiHeap::Alloc(NPrim & node)
{
	auto addr = addrs1[0]++;
	addrs2.push_back(addr);
	programs.insert(std::make_pair(addr, std::make_pair(std::make_shared<NPrim>(node.name, node.prim), node.name)));
	++allocTimes;
	return addr;
}

Addr
TiState::TiHeap::Alloc(std::shared_ptr<Node>& node)
{
	auto addr = addrs1[0]++;
	addrs2.push_back(addr);
	programs.insert(std::make_pair(addr, std::make_pair(node, std::string(""))));
	++allocTimes;
	return addr;
}

TiState::TiHeap &
TiState::TiHeap::Update(Addr addr, std::shared_ptr<Node>& newNode)
{
	programs.at(addr) = std::make_pair(newNode, std::string());
	++updateTimes;
	return *this;
}

TiState::TiHeap &
TiState::TiHeap::Free(Addr addr)
{
	addrs2.remove_if([&addr](Addr tar) {return tar == addr; });
	++freeTimes;
	return *this;
}

std::shared_ptr<Node>
TiState::TiHeap::LookUp(Addr addr)
{
	return programs.at(addr).first;
}

const std::list<Addr>&
TiState::TiHeap::Address() const
{
	return addrs2;
}

size_t
TiState::TiHeap::Size() const
{
	return programs.size();
}

int
TiState::TiHeap::GetAllocTimes() const
{
	return allocTimes;
}

int
TiState::TiHeap::GetUpdateTimes() const
{
	return updateTimes;
}

int
TiState::TiHeap::GetFreeTimes() const
{
	return freeTimes;
}

TiState::TiState()
{
}

TiState &
TiState::Init()
{
	m_stack.push_back(m_global.at("main"));
	return *this;
}

TiState::TiStack &
TiState::GetStack()
{
	return m_stack;
}

TiState::TiDump &
TiState::GetDump()
{
	return m_dump;
}

TiState::TiGlobal &
TiState::GetGlobal()
{
	return m_global;
}

TiState::TiHeap &
TiState::GetHeap()
{
	return m_heap;
}

TiState::TiStats &
TiState::GetStats()
{
	return m_stats;
}

TiState &
NAp::Advance(TiState & state)
{
	auto& stack = state.GetStack();
	auto& heap = state.GetHeap();
	try
	{
		auto& ptr = heap.LookUp(right);
		auto nind_ptr = std::dynamic_pointer_cast<NInd>(ptr);
		auto new_right = nind_ptr->addr;
		auto new_node = std::make_shared<NAp>(left, new_right);
		heap.Update(stack.back(), std::dynamic_pointer_cast<Node>(new_node));
	}
	catch (...)
	{
		;
	}
	stack.push_back(left);
	return state;
}


void
NSC::GetArgs(TiState & state)
{
	TiState::TiGlobal argList;
	auto& stack = state.GetStack();
	if (params.size() == 0)
	{
		return;
	}
	for (auto& name : params)
	{
		if (state.GetStack().size() == 0)
		{
			throw std::string("Run ") + name + " error! Not have enough params.";
		}

		auto& heap = state.GetHeap();
		auto operand = std::dynamic_pointer_cast<NAp>(heap.LookUp(stack.back()))->right;
		stack.pop_back();
		state.GetGlobal().insert({ name, operand });
	}
}

TiState &
NSC::Advance(TiState & state)
{
	auto& stack = state.GetStack();
	auto addr = stack.back();
	stack.pop_back();

	GetArgs(state);

	body->InstantiateAndUpdate(state, addr);
	stack.push_back(addr);

	return state;
}

TiState & NNum::Advance(TiState & state)
{
	auto& dump = state.GetDump();
	if (dump.size() == 0)
	{
		throw "Length of dump should't be zero!\n";
	}
	auto stack = dump.back();
	dump.pop_back();

	auto& heap = state.GetHeap();
	if (stack.size() == 2)
	{
		auto unaryOp = stack.back();
		auto ap = stack.front();
		auto newAp = std::make_shared<NAp>(unaryOp, state.GetStack().back());
		heap.Update(ap, std::dynamic_pointer_cast<Node>(newAp));
		state.GetStack().pop_back();
		state.GetStack().push_back(ap);
		state.GetStack().push_back(unaryOp);
	}
	else if(stack.size() == 3)
	{
		auto binOp = stack.back(); stack.pop_back();
		auto ap1 = stack.back(); stack.pop_back();
		auto ap2 = stack.back();
		auto newAp = std::make_shared<NAp>(binOp, state.GetStack().back());
		heap.Update(ap1, std::dynamic_pointer_cast<Node>(newAp));
		state.GetStack().pop_back();
		state.GetStack().push_back(ap2);
		state.GetStack().push_back(ap1);
		state.GetStack().push_back(binOp);
	}
	else
	{
		throw "NNum : length of stack too long!\n";
	}

	return state;
}
