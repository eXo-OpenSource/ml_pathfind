#pragma once
#include "include/ILuaModuleManager.h"
#include "JobManager.h"
#include <pathfind/AStar.h>
#include <unordered_set>

class CFunctions;
extern ILuaModuleManager10* pModuleManager;

extern JobManager<pathfind::AStarResult, 2> jobManager;

class CFunctions
{
public:
	static int FindShortestPathBetween(lua_State* luaVM);

	static std::unordered_set<lua_State*> _luaStates;
};
