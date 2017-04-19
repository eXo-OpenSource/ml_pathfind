#include "Module.h"
#include <iostream>
#include <chrono>

#include <pathfind/GraphReader.h>
#include <pathfind/Graph.h>
#include "include/ILuaModuleManager.h"

Module* g_Module = nullptr;

constexpr std::size_t kNumWorkers = 2;

Module::Module(ILuaModuleManager* manager) : _moduleManager(manager), _jobManager(kNumWorkers), _lastGraphId(0)
{
}

Module::~Module()
{
	// Shutdown job manager threads
	_jobManager.Stop();
}

void Module::Start()
{
	// Start job manager worker threads
	_jobManager.Start();
}

void Module::Process()
{
	// Call complete callbacks on main thread
	_jobManager.SpreadResults();
}

GraphId Module::LoadGraph(const std::string& path)
{
	// Make sure job manager is not running
	_jobManager.Stop();

	// Load graph
	auto startTime = std::chrono::system_clock::now();
	pathfind::GraphReader graphReader(path);

	// Find free graphId and add it
	AddGraph(++_lastGraphId, graphReader.Read());

	_moduleManager->Printf("Loaded graph! (Took %lldms)\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count());

	// Start job manager again
	_jobManager.Start();

	return _lastGraphId;
}

void Module::UnloadGraph(GraphId graphId)
{
	// Make sure job manager is not running
	_jobManager.Stop();

	// Delete graph reference
	RemoveGraph(graphId);

	// Start job manager again
	_jobManager.Start();
}