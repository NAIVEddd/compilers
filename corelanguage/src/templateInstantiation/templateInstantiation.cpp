#include"templateInstantiation/templateInstantiation.h"
#include<map>
#include<memory>
#include<functional>

std::vector<NPrim>& getPrDefs()
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
	mapAccuml(std::function<std::pair<TiState::TiHeap,TiState::TiGlobal>(TiState::TiHeap, std::shared_ptr<ScDef>&)> f, 
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

std::pair<TiState::TiHeap, TiState::TiGlobal> buildInitialHeap(std::shared_ptr<program>& prog)
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

std::string run(std::string & sourceCode)
{
	return std::string();
}

TiState compile(std::shared_ptr<program> & prog)
{
	TiState res;
	auto heap_global = buildInitialHeap(prog);
	res.m_heap = std::move(heap_global.first);
	res.m_global = std::move(heap_global.second);
	return res.Init();
}

std::vector<TiState> eval(TiState & initState)
{
	return std::vector<TiState>();
}

std::string showResult(std::vector<TiState>& theResult)
{
	return std::string();
}

TiState::TiState()
{
}

TiState & TiState::Init()
{
	m_stack.push_back(m_global.at("main"));
	return *this;
}

TiState::TiStack & TiState::GetStack()
{
	// TODO: insert return statement here
}

TiState::TiDump & TiState::GetDump()
{
	// TODO: insert return statement here
}

TiState::TiGlobal & TiState::GetGlobal()
{
	// TODO: insert return statement here
}

TiState::TiHeap & TiState::GetHeap()
{
	// TODO: insert return statement here
}

TiState::TiStats & TiState::GetStats()
{
	// TODO: insert return statement here
}
