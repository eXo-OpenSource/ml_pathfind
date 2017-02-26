#include <iostream>
#include <chrono>

#include <pathfind/GraphReader.h>
#include <pathfind/Graph.h>
#include <pathfind/AStar.h>

int main(int argc, char* argv[])
{
	// Read graph from file
	std::cout << "Loading graph from json file..." << std::endl;
	pathfind::GraphReader graphReader("../test/sa_nodes.json");
	auto start = std::chrono::system_clock::now();
	std::unique_ptr<pathfind::Graph> graph = graphReader.Read();
	std::cout << "Done! (Took " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms)" << std::endl;

	// Create instance of AStar
	start = std::chrono::system_clock::now();
	pathfind::AStar pathfind(graph.get(), Vector3(-2427.625, -2474.75, 35.75), Vector3(0.0f, 0.0f, 0.0f));
	std::cout << "Found a start node in " << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() << "micro s" << std::endl;

	// Run algorithm
	start = std::chrono::system_clock::now();
	std::vector<pathfind::GraphNode*> path = pathfind.CalculateShortestPath();
	std::cout << "Found a route in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count() << "ms" << std::endl;

	// Print path
	std::cout << "Calculating path: \n";
	for (pathfind::GraphNode* node : path)
	{
		std::cout << "Node " << node->id << " (" << node->position.GetX() << ", " << node->position.GetY() << ", " << node->position.GetZ() << ")\n";
	}

	if (path.empty())
		std::cout << "No path found";

	std::cout << std::endl;

	std::cin.get();
	return 0;
}
