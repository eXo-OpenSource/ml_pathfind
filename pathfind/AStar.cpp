#include "AStar.h"
#include "Graph.h"
#include <algorithm>
#include <limits>

namespace pathfind
{

AStar::AStar(Graph* graph, GraphNode* from, GraphNode* to) : _graph(graph), _from(from), _to(to)
{
}

AStar::AStar(Graph* graph, const Vector3& from, const Vector3& to)
	: AStar(graph, graph->FindClosestNodeTo(from), graph->FindClosestNodeTo(to))
{
}

AStarResult AStar::CalculateShortestPath()
{
	// Add the node we start from to the to-traverse list
	_openSet.insert(_from);

	// Cache for calculated g scores (calculated costs to <key>)
	_gScore[_from] = 0; // Costs for the from node are obviously 0

	// Cache for the calculated heuristics
	_fScore[_from] = HeuristicCostEstimate(_from, _to);

	// Traverse graph (and apply A*)
	while (!_openSet.empty())
	{
		GraphNode* current = GetMinimumInOpenSet();
		if (current == _to)
			return ReconstructPath();

		// We found a new node, so remove from open list and add to closed list
		_openSet.erase(current);
		_closedSet.insert(current);

		for (auto& neighbor : current->edges)
		{
			GraphNode* neighborNode = neighbor.first;
			std::size_t neighborDistance = neighbor.second;

			// If neighbor is already traversed, ignore this
			if (_closedSet.find(neighborNode) != _closedSet.end())
				continue;

			// Calculate tentative g score
			auto tentativeGScore = _gScore[current] + neighborDistance;
			if (_openSet.find(neighborNode) == _openSet.end())
				_openSet.insert(neighborNode);
			else if (tentativeGScore >= _gScore[neighborNode])
				continue;

			// We found the best path so far
			_cameFrom[neighborNode] = current;
			_gScore[neighborNode] = tentativeGScore;
			_fScore[neighborNode] = static_cast<float>(_gScore[neighborNode]) + HeuristicCostEstimate(neighborNode, _to);
		}
	}

	return {};
}

float AStar::HeuristicCostEstimate(GraphNode* from, GraphNode* to)
{
	return (to->position - from->position).Length();
}

GraphNode* AStar::GetMinimumInOpenSet()
{
	GraphNode* minNode = nullptr;
	float minDist = std::numeric_limits<float>::infinity();

	for (GraphNode* node : _openSet)
	{
		float fScore = _fScore[node];

		if (fScore < minDist)
		{
			minDist = fScore;
			minNode = node;
		}
	}

	return minNode;
}

std::vector<GraphNode*> AStar::ReconstructPath()
{
	std::vector<GraphNode*> path = { _from };
	std::size_t index = _cameFrom.size();
	path.resize(index);

	GraphNode* parent = _to;
	while (parent != _from)
	{
		path[--index] = parent;
		parent = _cameFrom[parent];
	}

	return std::move(path);
}

}
