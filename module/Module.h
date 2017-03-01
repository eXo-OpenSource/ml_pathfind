#pragma once
#include <memory>
#include <unordered_set>

#include <pathfind/AStar.h>
#include "JobManager.h"

struct lua_State;
class ILuaModuleManager;

class Module
{
public:
	Module(ILuaModuleManager* manager);
	~Module();

	void Start();
	void Process();

	void LoadGraph(const std::string& path);
	void UnloadGraph();

	inline void AddLuaVM(lua_State* luaVM) { _luaStates.insert(luaVM); }
	inline void RemoveLuaVM(lua_State* luaVM) { _luaStates.erase(luaVM); }
	inline bool HasLuaVM(lua_State* luaVM) { return _luaStates.find(luaVM) != _luaStates.end(); }

	inline JobManager<pathfind::AStarResult>& GetJobManager() { return _jobManager; }

	inline pathfind::Graph* GetGraph() { return _graph.get(); }

private:
	ILuaModuleManager* _moduleManager;

	JobManager<pathfind::AStarResult> _jobManager;

	std::unique_ptr<pathfind::Graph> _graph;

	std::unordered_set<lua_State*> _luaStates;
};

extern Module* g_Module;
