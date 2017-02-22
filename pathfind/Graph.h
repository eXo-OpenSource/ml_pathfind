#pragma once
#include <cstddef>
#include <vector>
#include "Vector3.h"

namespace pathfind
{
using GraphNodeId = std::size_t;

struct GraphNode
{
	GraphNodeId id;
	Vector3 position;
	std::vector<std::pair<GraphNode*, std::size_t>> edges;
};

class Graph
{
	friend class GraphReader;

public:
	Graph();
	~Graph();

	GraphNode* FindClosestNodeTo(const Vector3& position);

private:
	std::vector<GraphNode> _nodes;
};

}
