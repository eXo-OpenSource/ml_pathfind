#pragma once
#include "include/ILuaModuleManager.h"
#include "JobManager.h"
#include <pathfind/AStar.h>
#include <unordered_set>

class CFunctions;
extern ILuaModuleManager10* pModuleManager;

extern JobManager<pathfind::AStarResult> jobManager;
extern std::unique_ptr<pathfind::Graph> graph;

class CFunctions
{
public:
	static int FindShortestPathBetween(lua_State* luaVM);

	static std::unordered_set<lua_State*> _luaStates;
};
