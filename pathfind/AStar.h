#pragma once
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Vector3.h"

namespace pathfind
{
class Graph;
struct GraphNode;

using AStarResult = std::vector<GraphNode*>;

class AStar
{
public:
	AStar(Graph* graph, GraphNode* from, GraphNode* to);
	AStar(Graph* graph, const Vector3& from, const Vector3& to);

	AStarResult CalculateShortestPath();

protected:
	float HeuristicCostEstimate(GraphNode* from, GraphNode* to);

	GraphNode* GetMinimumInOpenSet();

	std::vector<GraphNode*> ReconstructPath();

private:
	Graph* _graph;
	GraphNode* _to;
	GraphNode* _from;

	std::unordered_set<GraphNode*> _closedSet;
	std::unordered_set<GraphNode*> _openSet;

	std::unordered_map<GraphNode*, std::size_t> _gScore;
	std::unordered_map<GraphNode*, float> _fScore;

	std::unordered_map<GraphNode*, GraphNode*> _cameFrom;
};

}
