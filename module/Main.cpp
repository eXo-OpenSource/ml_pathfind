#include "Common.h"
#include "CFunctions.h"
#include "include/ILuaModuleManager.h"
#include "JobManager.h"
#include <cstring>

ILuaModuleManager10* pModuleManager = nullptr;
JobManager<pathfind::AStarResult, 2> jobManager;

// Initialisation function (module entrypoint)
MTAEXPORT bool InitModule(ILuaModuleManager10* pManager, char* szModuleName, char* szAuthor, float* fVersion)
{
    pModuleManager = pManager;

    // Set the module info
    std::memcpy(szModuleName, "Pathfind Mmodule", MAX_INFO_LENGTH);
    std::memcpy(szAuthor, "Jusonex", MAX_INFO_LENGTH);
    *fVersion = 1.0f;

	// Start job manager worker threads
	jobManager.Start();

    return true;
}

MTAEXPORT void RegisterFunctions(lua_State* luaVM)
{
    if (pModuleManager && luaVM)
    {
		// Add lua vm to states list (to check validility)
		CFunctions::_luaStates.insert(luaVM);

        pModuleManager->RegisterFunction(luaVM, "findShortestPathBetween", &CFunctions::FindShortestPathBetween);
    }
}

MTAEXPORT bool DoPulse()
{
	// Call complete callbacks on main thread
	jobManager.SpreadResults();

    return true;
}

MTAEXPORT bool ShutdownModule()
{
    return true;
}

MTAEXPORT bool ResourceStopping(lua_State* luaVM)
{
	// Invalidate lua vm by removing it from the valid list
	CFunctions::_luaStates.erase(luaVM);

    return true;
}

MTAEXPORT bool ResourceStopped(lua_State* luaVM)
{
    return true;
}