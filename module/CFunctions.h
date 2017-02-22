/*********************************************************
*
*  vRP GPS module
*
*********************************************************/
#pragma once
#include "include/ILuaModuleManager.h"

class CFunctions;
extern ILuaModuleManager10* pModuleManager;

class CFunctions
{
public:
    static int calculateRouteBetweenPoints(lua_State* luaVM);
    static int processGPSEvents(lua_State* luaVM );
};
