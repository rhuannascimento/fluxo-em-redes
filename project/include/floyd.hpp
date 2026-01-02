#pragma once

#include <vector>
#include "graph.hpp"

struct FloydResult {
    std::vector<std::vector<long long>> dist;
    std::vector<std::vector<Graph::VertexId>> pred;
    bool has_negative_cycle{false};
};

FloydResult floyd_warshall(const Graph& g);
