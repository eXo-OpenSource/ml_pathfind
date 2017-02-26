#include "Module.h"
#include <iostream>
#include <chrono>

#include <pathfind/GraphReader.h>
#include <pathfind/Graph.h>

Module* g_Module = nullptr;

constexpr std::size_t kNumWorkers = 2;
constexpr const char* kGraphPath = "sa_nodes.json";

Module::Module() : _jobManager(kNumWorkers)
{
	// Load graph
	auto startTime = std::chrono::system_clock::now();
	pathfind::GraphReader graphReader(kGraphPath);
	_graph = graphReader.Read();
	std::cout << "Loaded graph! (Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - startTime).count() << "ms)" << std::endl;
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
