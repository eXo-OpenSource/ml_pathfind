#pragma once

class ILuaModuleManager10;
struct lua_State;

extern ILuaModuleManager10* pModuleManager;


class CFunctions
{
public:
	static int FindShortestPathBetween(lua_State* luaVM);
};
