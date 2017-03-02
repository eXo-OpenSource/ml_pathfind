#pragma once

class ILuaModuleManager10;
struct lua_State;

extern ILuaModuleManager10* pModuleManager;


class CFunctions
{
public:
	static int LoadPathGraph(lua_State* luaVM);
	static int UnloadPathGraph(lua_State* luaVM);
	static int FindShortestPathBetween(lua_State* luaVM);
	static int IsGraphLoaded(lua_State* luaVM);
};
