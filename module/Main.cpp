/*********************************************************
*
*  PROJECT:      vRoleplay
*  FILE:         Main.cpp
*  PURPOSE:      Entry point file
*
*********************************************************/
#include "Common.h"
#include "WayFinderJobManager.h"
#include "CFunctions.h"
#include "include/ILuaModuleManager.h"

ILuaModuleManager10* pModuleManager = nullptr;

// Initialisation function (module entrypoint)
MTAEXPORT bool InitModule(ILuaModuleManager10* pManager, char* szModuleName, char* szAuthor, float* fVersion)
{
    pModuleManager = pManager;

    // Set the module info
    memcpy ( szModuleName, "Pathfind module", MAX_INFO_LENGTH );
    memcpy ( szAuthor, "Jusonex", MAX_INFO_LENGTH );
    *fVersion = 1.0f;

    // Initialise way finder job manager
    new WayFinderJobManager;

    return true;
}

MTAEXPORT void RegisterFunctions(lua_State* luaVM)
{
    if (pModuleManager && luaVM)
    {
        pModuleManager->RegisterFunction(luaVM, "calculateRouteBetweenPoints", CFunctions::calculateRouteBetweenPoints);
    }
}

MTAEXPORT bool DoPulse()
{
    return true;
}

MTAEXPORT bool ShutdownModule(void)
{
    WayFinderJobManager::instance().stop();
    return true;
}

MTAEXPORT bool ResourceStopping(lua_State* luaVM)
{
    return true;
}

MTAEXPORT bool ResourceStopped(lua_State* luaVM)
{
    return true;
}