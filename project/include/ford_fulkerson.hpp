#pragma once

#include "graph.hpp"

struct FordFulkersonResult {
    long long max_flow;
    Graph flow_graph;
};

FordFulkersonResult ford_fulkerson(const Graph& g, Graph::VertexId source, Graph::VertexId sink);
