#pragma once

#include <string>
#include <vector>
#include "graph.hpp"

struct FlowResultEntry {
    Graph::VertexId from;
    Graph::VertexId to;
    int capacity;
    int flow;
    int residual;
};

struct FlowReport {
    int max_flow;
    std::vector<FlowResultEntry> entries;
};

Graph load_graph_from_file(const std::string& path);
FlowReport build_flow_report(const Graph& g);
void write_report_to_file(const std::string& path, const FlowReport& report);
void write_dot(const std::string& path, const Graph& g);
