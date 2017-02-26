#include "Module.h"
#include <iostream>
#include <chrono>

#include <pathfind/GraphReader.h>
#include <pathfind/Graph.h>
#include "include/ILuaModuleManager.h"

Module* g_Module = nullptr;

constexpr std::size_t kNumWorkers = 2;
constexpr const char* kGraphPath = "sa_nodes.json";

Module::Module(ILuaModuleManager* manager) : _moduleManager(manager), _jobManager(kNumWorkers)
{
	// Load graph
	auto startTime = std::chrono::system_clock::now();
	pathfind::GraphReader graphReader(kGraphPath);
	_graph = graphReader.Read();
	_moduleManager->Printf("Loaded graph! (Took %dms)\n", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count());
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
