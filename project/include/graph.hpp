#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "vertex.hpp"
#include "edge.hpp"

class Graph {
public:
    using VertexId = Vertex::Id;
    using EdgeId = Edge::Id;

    Graph() = default;

    VertexId add_vertex(const std::string& label = "");
    EdgeId add_edge(VertexId from, VertexId to, int cost = 0);

    const Vertex* get_vertex(VertexId id) const;
    const Edge* get_edge(EdgeId id) const;

    std::vector<VertexId> vertices() const;
    std::vector<EdgeId> edges() const;

    std::vector<EdgeId> outgoing(VertexId v) const;
    std::vector<EdgeId> incoming(VertexId v) const;

    bool has_vertex(VertexId id) const;
    bool has_edge(EdgeId id) const;

    int edge_cost(EdgeId id) const;
    void set_edge_cost(EdgeId id, int cost);

private:
    VertexId next_vertex_id_ {0};
    EdgeId next_edge_id_ {0};

    std::unordered_map<VertexId, std::unique_ptr<Vertex>> vertices_;
    std::unordered_map<EdgeId, std::unique_ptr<Edge>> edges_;

    std::unordered_map<VertexId, std::vector<EdgeId>> adj_out_;
    std::unordered_map<VertexId, std::vector<EdgeId>> adj_in_;
};
