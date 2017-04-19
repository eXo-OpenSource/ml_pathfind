#pragma once
#include <memory>
#include <unordered_set>

#include <pathfind/Graph.h>
#include <pathfind/AStar.h>
#include "JobManager.h"

struct lua_State;
class ILuaModuleManager;
using GraphId = size_t;

class Module
{
public:
	Module(ILuaModuleManager* manager);
	~Module();

	void Start();
	void Process();

	GraphId LoadGraph(const std::string& path);
	void UnloadGraph(GraphId graphId);

	inline void AddLuaVM(lua_State* luaVM) { _luaStates.insert(luaVM); }
	inline void RemoveLuaVM(lua_State* luaVM) { _luaStates.erase(luaVM); }
	inline bool HasLuaVM(lua_State* luaVM) { return _luaStates.find(luaVM) != _luaStates.end(); }
	inline pathfind::Graph* GetGraph(GraphId graphId) {
		if (_graphs.find(graphId) != _graphs.end()) {
			return _graphs.at(graphId).get();
		}

		return nullptr;
	}
	inline void AddGraph(GraphId graphId, std::unique_ptr<pathfind::Graph> graph) { _graphs[graphId] = std::move(graph); }
	inline void RemoveGraph(GraphId graphId) { _graphs.erase(graphId); }

	inline JobManager<pathfind::AStarResult>& GetJobManager() { return _jobManager; }

private:
	ILuaModuleManager* _moduleManager;
	JobManager<pathfind::AStarResult> _jobManager;
	std::unordered_map<GraphId, std::unique_ptr<pathfind::Graph>> _graphs;
	std::unordered_set<lua_State*> _luaStates;
	GraphId _lastGraphId;
};

extern Module* g_Module;
