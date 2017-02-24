#include "Common.h"
#include "CFunctions.h"
#include "include/ILuaModuleManager.h"
#include "JobManager.h"
#include <cstring>
#include <pathfind/GraphReader.h>
#include <pathfind/Graph.h>

ILuaModuleManager10* pModuleManager = nullptr;
JobManager<pathfind::AStarResult> jobManager(2);
std::unique_ptr<pathfind::Graph> graph;

// Initialisation function (module entrypoint)
MTAEXPORT bool InitModule(ILuaModuleManager10* pManager, char* szModuleName, char* szAuthor, float* fVersion)
{
    pModuleManager = pManager;

    // Set the module info
    std::memcpy(szModuleName, "Pathfind Module", MAX_INFO_LENGTH);
    std::memcpy(szAuthor, "Jusonex", MAX_INFO_LENGTH);
    *fVersion = 1.0f;

	// Load graph
	pathfind::GraphReader graphReader("sa_nodes.json");
	graph = graphReader.Read();

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