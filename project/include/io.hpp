#pragma once

#include <string>
#include <vector>
#include "graph.hpp"

Graph load_graph_from_file(const std::string& path);
void write_dot(const std::string& path, const Graph& g);
