#include "Graph.h"
#include <limits>

namespace pathfind
{

Graph::Graph()
{

}

Graph::~Graph()
{

}

GraphNode* Graph::FindClosestNodeTo(const Vector3& position)
{
	// Iterate through all nodes and calculate the minimum distance
	// TODO: Optimize this with index structures
	GraphNode* closestNode = nullptr;
	float closestDist = std::numeric_limits<float>::infinity();

	for (GraphNode& node : _nodes)
	{
		float dist = (node.position - position).LengthSquared();
		if (dist < closestDist)
		{
			closestNode = &node;
			closestDist = dist;
		}
	}

	return closestNode;
}

}
