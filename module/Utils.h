#pragma once
#include "extra/CLuaArguments.h"
#include <pathfind/Graph.h>

class Utils
{

public:
#ifdef DEBUG
	static const int MAX_NODE_DEPTH = std::numeric_limits<int>::max();
#else
	static const int MAX_NODE_DEPTH = 15;
#endif

	static void GetNodeNeighbors(lua_State* luaVM, pathfind::GraphNode* node, int depth);

};