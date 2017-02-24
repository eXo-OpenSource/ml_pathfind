#include <iostream>
#include <pathfind/GraphReader.h>
#include <pathfind/Graph.h>
#include <pathfind/AStar.h>

int main(int argc, char* argv[])
{
	// Read graph from file
	pathfind::GraphReader graphReader("../test/sa_nodes.json");
	std::cout << "Loading graph from json file" << std::endl;
	std::unique_ptr<pathfind::Graph> graph = graphReader.Read();
	std::cout << "Done loading!" << std::endl;

	// Create instance of AStar
	pathfind::AStar pathfind(graph.get(), Vector3(-2427.625, -2474.75, 35.75), Vector3(0.0f, 0.0f, 0.0f));

	// Run algorithm
	std::vector<pathfind::GraphNode*> path = pathfind.CalculateShortestPath();

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
