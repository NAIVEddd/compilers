#include"expr.h"
#include"syntax.h"
#include"templateInstantiation/templateInstantiation.h"

Addr 
EVar::Instantiate(TiState & state)
{
	return state.GetGlobal().at(name);
}

Addr 
ENum::Instantiate(TiState & state)
{
	return state.GetHeap().Alloc(std::dynamic_pointer_cast<Node>(std::make_shared<NNum>(num)));
}

Addr 
EConstr::Instantiate(TiState & state)
{
	throw "Can't instantiate data constructor yet.\n";
	return Addr();
}

Addr 
EAp::Instantiate(TiState & state)
{
	auto addr1 = left->Instantiate(state);
	auto addr2 = right->Instantiate(state);
	return state.GetHeap().Alloc(std::dynamic_pointer_cast<Node>(std::make_shared<NAp>(addr1, addr2)));
}

Addr 
ELet::Instantiate(TiState & state)
{

	return Addr();
}

void 
ELet::InstantiateDefs(TiState & state)
{
}