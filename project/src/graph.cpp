#include "graph.hpp"

Vertex::Id Graph::add_vertex(const std::string& label) {
    const auto id = next_vertex_id_++;
    vertices_.emplace(id, std::make_unique<Vertex>(id, label));
    adj_out_.emplace(id, std::vector<EdgeId>{});
    adj_in_.emplace(id, std::vector<EdgeId>{});
    return id;
}

Edge::Id Graph::add_edge(VertexId from, VertexId to, int capacity) {
    const auto id = next_edge_id_++;
    edges_.emplace(id, std::make_unique<Edge>(id, from, to, capacity, 0));
    adj_out_[from].push_back(id);
    adj_in_[to].push_back(id);
    return id;
}

const Vertex* Graph::get_vertex(VertexId id) const {
    auto it = vertices_.find(id);
    if (it == vertices_.end()) return nullptr;
    return it->second.get();
}

const Edge* Graph::get_edge(EdgeId id) const {
    auto it = edges_.find(id);
    if (it == edges_.end()) return nullptr;
    return it->second.get();
}

std::vector<Graph::VertexId> Graph::vertices() const {
    std::vector<VertexId> v;
    v.reserve(vertices_.size());
    for (const auto& kv : vertices_) v.push_back(kv.first);
    return v;
}

std::vector<Graph::EdgeId> Graph::edges() const {
    std::vector<EdgeId> e;
    e.reserve(edges_.size());
    for (const auto& kv : edges_) e.push_back(kv.first);
    return e;
}

std::vector<Graph::EdgeId> Graph::outgoing(VertexId v) const {
    auto it = adj_out_.find(v);
    if (it == adj_out_.end()) return {};
    return it->second;
}

std::vector<Graph::EdgeId> Graph::incoming(VertexId v) const {
    auto it = adj_in_.find(v);
    if (it == adj_in_.end()) return {};
    return it->second;
}

bool Graph::has_vertex(VertexId id) const {
    return vertices_.find(id) != vertices_.end();
}

bool Graph::has_edge(EdgeId id) const {
    return edges_.find(id) != edges_.end();
}

void Graph::set_edge_capacity(EdgeId id, int capacity) {
    auto it = edges_.find(id);
    if (it != edges_.end()) it->second->set_capacity(capacity);
}

int Graph::edge_capacity(EdgeId id) const {
    auto it = edges_.find(id);
    if (it == edges_.end()) return 0;
    return it->second->capacity();
}

int Graph::edge_flow(EdgeId id) const {
    auto it = edges_.find(id);
    if (it == edges_.end()) return 0;
    return it->second->flow();
}

void Graph::set_edge_flow(EdgeId id, int flow) {
    auto it = edges_.find(id);
    if (it != edges_.end()) it->second->set_flow(flow);
}
