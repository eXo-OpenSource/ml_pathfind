#include "Utils.h"

void Utils::GetNodeNeighbors(lua_State* luaVM, pathfind::GraphNode* startNode, int depth)
{
	std::size_t index = 1;

	for (const auto& pair : startNode->edges)
	{	
		pathfind::GraphNode* edgeNode = pair.first;

		lua_pushnumber(luaVM, index++);
			lua_newtable(luaVM); 

#ifdef DEBUG
				lua_pushstring(luaVM, "nodeId");
#else
				lua_pushnumber(luaVM, 1);
#endif
				lua_pushnumber(luaVM, edgeNode->id);
				lua_settable(luaVM, -3);

#ifdef DEBUG
				lua_pushstring(luaVM, "posX");
#else
				lua_pushnumber(luaVM, 2);
#endif
				lua_pushnumber(luaVM, edgeNode->position.GetX());
				lua_settable(luaVM, -3);

#ifdef DEBUG
				lua_pushstring(luaVM, "posY");
#else
				lua_pushnumber(luaVM, 3);
#endif
				lua_pushnumber(luaVM, edgeNode->position.GetY());
				lua_settable(luaVM, -3);

#ifdef DEBUG
				lua_pushstring(luaVM, "posZ");
#else
				lua_pushnumber(luaVM, 4);
#endif
				lua_pushnumber(luaVM, edgeNode->position.GetZ());
				lua_settable(luaVM, -3);

#ifdef DEBUG
				lua_pushstring(luaVM, "edges");
#else
				lua_pushnumber(luaVM, 5);
#endif
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
}
