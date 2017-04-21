#include "Utils.h"
#include "CFunctions.h"

bool Utils::GetNodeNeighbors(lua_State* luaVM, pathfind::GraphNode* startNode, int depth)
{
	if (startNode == nullptr) {
		return false;
	}


	std::size_t index = 1;

	for (const auto& pair : startNode->edges)
	{	
		pathfind::GraphNode* edgeNode = pair.first;

		lua_pushnumber(luaVM, index++);
			lua_newtable(luaVM); 
				lua_pushnumber(luaVM, 1);
				lua_pushnumber(luaVM, edgeNode->id);
				lua_settable(luaVM, -3);

				lua_pushnumber(luaVM, 2);
				lua_pushnumber(luaVM, edgeNode->position.GetX());
				lua_settable(luaVM, -3);

				lua_pushnumber(luaVM, 3);
				lua_pushnumber(luaVM, edgeNode->position.GetY());
				lua_settable(luaVM, -3);

				lua_pushnumber(luaVM, 4);
				lua_pushnumber(luaVM, edgeNode->position.GetZ());
				lua_settable(luaVM, -3);

				lua_pushnumber(luaVM, 5);
					if (depth > 0) {
						lua_newtable(luaVM);
							GetNodeNeighbors(luaVM, edgeNode, depth - 1);
					}
					else {
						lua_pushboolean(luaVM, false);
					}
				lua_settable(luaVM, -3);

		lua_settable(luaVM, -3);
	}

	return true;
}