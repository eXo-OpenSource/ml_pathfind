#include "CFunctions.h"
#include "include/ILuaModuleManager.h"
#include "extra/CLuaArguments.h"

#include <pathfind/Graph.h>
#include "Module.h"

#ifndef _WIN32
	#include <sys/stat.h>
#endif

int CFunctions::LoadPathGraph(lua_State* luaVM)
{
	if (lua_type(luaVM, 1) != LUA_TSTRING)
	{
		pModuleManager->ErrorPrintf("Bad argument @ loadPathGraph\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	// Build graph path
	char buf[300];
	if (!pModuleManager->GetResourceFilePath(luaVM, lua_tostring(luaVM, 1), buf, sizeof(buf)))
	{
		pModuleManager->ErrorPrintf("File does not exist @ loadPathGraph\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	// Check if path is valid
	std::string path{ buf };
	if (path.find("..") != std::string::npos)
	{
		pModuleManager->ErrorPrintf("Bad path @ loadPathGraph\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	// Load graph
	GraphId graphId = g_Module->LoadGraph(path);
	lua_pushnumber(luaVM, graphId);

	return 1;
}

int CFunctions::UnloadPathGraph(lua_State* luaVM)
{
	if (lua_type(luaVM, 1) != LUA_TNUMBER) {
		pModuleManager->ErrorPrintf("Bad argument @ unloadPathGraph\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	// Check if graph has been loaded
	GraphId graphId = (GraphId) lua_tonumber(luaVM, 1);
	if (g_Module->GetGraph(graphId) == nullptr) {
		pModuleManager->ErrorPrintf("Bad graph @ unloadPathGraph, cannot unload\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	g_Module->UnloadGraph(graphId);
	lua_pushboolean(luaVM, true);
	return 1;
}

int CFunctions::FindShortestPathBetween(lua_State* luaVM)
{
	// bool findShortestPathBetween(int graphId, float startX, float startY, float startZ, float endX, float endY, float endZ, function callback)
	if (lua_type(luaVM, 1) != LUA_TNUMBER || lua_type(luaVM, 2) != LUA_TNUMBER || lua_type(luaVM, 3) != LUA_TNUMBER || lua_type(luaVM, 4) != LUA_TNUMBER ||
		lua_type(luaVM, 5) != LUA_TNUMBER || lua_type(luaVM, 6) != LUA_TNUMBER || lua_type(luaVM, 7) != LUA_TNUMBER ||
		lua_type(luaVM, 8) != LUA_TFUNCTION)
	{
		pModuleManager->ErrorPrintf("Bad argument @ findShortestPathBetween\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	// Check if graph has been loaded
	GraphId graphId = (GraphId) lua_tonumber(luaVM, 1);
	if (g_Module->GetGraph(graphId) == nullptr)
	{
		pModuleManager->ErrorPrintf("No graph loaded @ findShortestPathBetween\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	// Make vectors from Lua primitives
	Vector3 from((float)lua_tonumber(luaVM, 1), (float)lua_tonumber(luaVM, 2), (float)lua_tonumber(luaVM, 3));
	Vector3 to((float)lua_tonumber(luaVM, 4), (float)lua_tonumber(luaVM, 5), (float)lua_tonumber(luaVM, 6));

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(luaVM, -1);
	int funcRef = luaL_ref(luaVM, LUA_REGISTRYINDEX);

	g_Module->GetJobManager().PushTask([from, to, graphId]() {

		// Prepare information we need to pass to A*
		pathfind::AStar algorithm(g_Module->GetGraph(graphId), from, to);
		
		// Run A*
		return algorithm.CalculateShortestPath();

	}, [luaVM = lua_getmainstate(luaVM), funcRef](const pathfind::AStarResult& result) {

		// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
		if (!g_Module->HasLuaVM(luaVM))
			return;

		// Push stored reference to callback function to the stack
		lua_rawgeti(luaVM, LUA_REGISTRYINDEX, funcRef);

		// Push arguments to the stack
		lua_newtable(luaVM);
		std::size_t index = 0;

		for (pathfind::GraphNode* node : result)
		{
			// Set table index
			lua_pushnumber(luaVM, ++index);

			// Create a table containing the vector components
			lua_newtable(luaVM);

			lua_pushnumber(luaVM, 1);
			lua_pushnumber(luaVM, node->position.GetX());
			lua_settable(luaVM, -3);

			lua_pushnumber(luaVM, 2);
			lua_pushnumber(luaVM, node->position.GetY());
			lua_settable(luaVM, -3);

			lua_pushnumber(luaVM, 3);
			lua_pushnumber(luaVM, node->position.GetZ());
			lua_settable(luaVM, -3);

			// Add vector table to result table
			lua_settable(luaVM, -3);
		}

		// Finally, call the function
		int err = lua_pcall(luaVM, 1, 0, 0);
		if (err != 0)
			pModuleManager->ErrorPrintf("%s\n", lua_tostring(luaVM, -1));

	});

	lua_pushboolean(luaVM, true);
	return 1;
}

int CFunctions::IsGraphLoaded(lua_State* luaVM)
{
	if (lua_type(luaVM, 1) != LUA_TNUMBER) {
		pModuleManager->ErrorPrintf("Bad argument @ unloadPathGraph\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	GraphId graphId = (GraphId) lua_tonumber(luaVM, 1);
	lua_pushboolean(luaVM, g_Module->GetGraph(graphId) != nullptr);
	return 1;
}

int CFunctions::FindNodeAt(lua_State* luaVM)
{
	if (lua_type(luaVM, 1) != LUA_TNUMBER || lua_type(luaVM, 2) != LUA_TNUMBER || lua_type(luaVM, 3) != LUA_TNUMBER || lua_type(luaVM, 4) != LUA_TNUMBER)
	{
		pModuleManager->ErrorPrintf("Bad argument @ findNodeAt\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	GraphId graphId = (GraphId) lua_tonumber(luaVM, 1);
	if (g_Module->GetGraph(graphId) == nullptr) {
		pModuleManager->ErrorPrintf("Bad graph @ findNodeAt\n");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	Vector3 position((float)lua_tonumber(luaVM, 2), (float)lua_tonumber(luaVM, 3), (float)lua_tonumber(luaVM, 4));
	pathfind::GraphNode* node = g_Module->GetGraph(graphId)->FindClosestNodeTo(position);
	if (!node) {
		lua_pushboolean(luaVM, false);
		return 1;
	}

	lua_pushnumber(luaVM, node->id);
	lua_pushnumber(luaVM, node->position.GetX());
	lua_pushnumber(luaVM, node->position.GetY());
	lua_pushnumber(luaVM, node->position.GetZ());

	return 4;
}