#pragma once
#include <string>
#include <memory>

namespace pathfind
{

class Graph;

class GraphReader
{
public:
	GraphReader(const std::string& path);

	std::unique_ptr<Graph> Read();

private:
	std::string _path;
};

}
