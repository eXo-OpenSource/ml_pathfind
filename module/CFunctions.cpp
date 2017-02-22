#include "CFunctions.h"
#include "extra/CLuaArguments.h"
#include <pathfind/Graph.h>

int CFunctions::FindShortestPathBetween(lua_State* luaVM)
{
	// findShortestPathBetween(float startX, float startY, float startZ, float endX, float endY, float endZ, function callback)
	if (lua_type(luaVM, 1) != LUA_TNUMBER || lua_type(luaVM, 2) != LUA_TNUMBER || lua_type(luaVM, 3) != LUA_TNUMBER ||
		lua_type(luaVM, 4) != LUA_TNUMBER || lua_type(luaVM, 5) != LUA_TNUMBER || lua_type(luaVM, 6) != LUA_TNUMBER)
	{
		pModuleManager->ErrorPrintf("Bad argument @ findShortestPathBetween");
		lua_pushboolean(luaVM, false);
		return 1;
	}

	// Make vectors from Lua primitives
	Vector3 from((float)lua_tonumber(luaVM, 1), (float)lua_tonumber(luaVM, 2), (float)lua_tonumber(luaVM, 3));
	Vector3 to((float)lua_tonumber(luaVM, 4), (float)lua_tonumber(luaVM, 5), (float)lua_tonumber(luaVM, 6));

	// Read Lua callback function from stack
	// TODO

	jobManager.PushTask([from, to]() {

		// Prepare information we need to pass to A*
		pathfind::AStar algorithm(nullptr, from, to); // TODO: Insert graph
		
		// Run A*
		return algorithm.CalculateShortestPath();

	}, [luaVM](const pathfind::AStarResult& result) {

		// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
		// TODO

		// Push to Lua stack
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

		lua_call(luaVM, 2, 0);

	});

	return 0;
}
