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
	inline GraphNode* GetNode(unsigned int id) {
		if (id >= _nodes.size()) {
			return nullptr;
		}

		return &_nodes[id]; 
	};

private:
	std::vector<GraphNode> _nodes;
};

}
