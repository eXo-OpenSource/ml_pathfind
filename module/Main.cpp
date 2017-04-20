#include "Common.h"
#include "CFunctions.h"
#include "include/ILuaModuleManager.h"
#include <cstring>
#include "Module.h"

#ifndef WIN32
	#include "luaimports/luaimports.linux.h"
#endif

ILuaModuleManager10* pModuleManager = nullptr;

// Initialisation function (module entrypoint)
MTAEXPORT bool InitModule(ILuaModuleManager10* pManager, char* szModuleName, char* szAuthor, float* fVersion)
{
#ifndef WIN32
	ImportLua();
#endif

    pModuleManager = pManager;

	// Set the module info
	std::memcpy(szModuleName, "Pathfind Module", MAX_INFO_LENGTH);
	std::memcpy(szAuthor, "Jusonex, StiviK", MAX_INFO_LENGTH);
	*fVersion = 1.0f;
	
	// Load module
	g_Module = new Module(pManager);
	g_Module->Start();

    return true;
}

MTAEXPORT void RegisterFunctions(lua_State* luaVM)
{
	if (!pModuleManager || !luaVM)
		return;

	// Add lua vm to states list (to check validility)
	g_Module->AddLuaVM(luaVM);

	// Register functions
	pModuleManager->RegisterFunction(luaVM, "loadPathGraph", &CFunctions::LoadPathGraph);
	pModuleManager->RegisterFunction(luaVM, "unloadPathGraph", &CFunctions::UnloadPathGraph);
	pModuleManager->RegisterFunction(luaVM, "findShortestPathBetween", &CFunctions::FindShortestPathBetween);
	pModuleManager->RegisterFunction(luaVM, "isGraphLoaded", &CFunctions::IsGraphLoaded);
	pModuleManager->RegisterFunction(luaVM, "findNodeAt", &CFunctions::FindNodeAt);
	pModuleManager->RegisterFunction(luaVM, "getNodeNeighbors", &CFunctions::GetNodeNeighbors);
}

MTAEXPORT bool DoPulse()
{
	g_Module->Process();

	return true;
}

MTAEXPORT bool ShutdownModule()
{
	// Unload module
	delete g_Module;

    return true;
}

MTAEXPORT bool ResourceStopping(lua_State* luaVM)
{
	// Invalidate lua vm by removing it from the valid list
	g_Module->RemoveLuaVM(luaVM);

	return true;
}

MTAEXPORT bool ResourceStopped(lua_State* luaVM)
{
	return true;
}
