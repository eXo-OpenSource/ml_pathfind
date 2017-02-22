#include "GraphReader.h"
#include <json/json.hpp>
#include <fstream>
#include "Graph.h"

namespace pathfind
{

GraphReader::GraphReader(const std::string& path) : _path(path)
{
}

std::unique_ptr<Graph> GraphReader::Read()
{
	// Load json file
	nlohmann::json json = nlohmann::json(std::ifstream(_path));

	// Create graph from nodes
	auto graph = std::make_unique<Graph>();
	for (auto& node : json["nodes"])
	{
		graph->_nodes.push_back(GraphNode{ node["id"], Vector3(node["x"], node["y"], node["z"]), {} });
	}

	// Fill in edge pointers
	std::size_t i = 0;
	for (auto& node : json["nodes"])
	{
		auto& edges = graph->_nodes[i++].edges;

		for (auto& edge : node["edges"])
		{
			auto edgeIndex = edge[0].get<std::size_t>();
			auto distance = edge[1].get<std::size_t>();

			edges.push_back({ &graph->_nodes[edgeIndex], distance });
		}
	}

	return graph;
}

}
