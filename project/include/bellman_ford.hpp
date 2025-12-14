#pragma once

#include "graph.hpp"
#include <vector>
#include <limits>

struct BellmanFordResult {
    std::vector<int> distances;
    std::vector<Graph::VertexId> predecessors;
    bool has_negative_cycle;
};

BellmanFordResult bellman_ford_recursive(const Graph& g, Graph::VertexId source);
