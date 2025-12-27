#include "graph.hpp"
#include <optional>
#include <algorithm>

Vertex::Id Graph::add_vertex(const std::string& label) {
    const auto id = next_vertex_id_++;
    vertices_.emplace(id, std::make_unique<Vertex>(id, label));
    adj_out_.emplace(id, std::vector<EdgeId>{});
    adj_in_.emplace(id, std::vector<EdgeId>{});
    resize_cost_matrix_for_new_vertex();
    return id;
}

Edge::Id Graph::add_edge(VertexId from, VertexId to, int cost) {
    const auto id = next_edge_id_++;
    edges_.emplace(id, std::make_unique<Edge>(id, from, to, cost));
    adj_out_[from].push_back(id);
    adj_in_[to].push_back(id);
    set_matrix_cost(from, to, static_cast<int>(cost));
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

int Graph::edge_cost(EdgeId id) const {
    auto it = edges_.find(id);
    if (it == edges_.end()) return 0;
    return it->second->cost();
}

void Graph::set_edge_cost(EdgeId id, int cost) {
    auto it = edges_.find(id);
    if (it != edges_.end()) it->second->set_cost(cost);
}

size_t Graph::num_vertices() const {
    return next_vertex_id_;
}

void Graph::resize_cost_matrix_for_new_vertex(int inf) {
    const size_t n = num_vertices();
    if (cost_matrix_.size() >= n) return;

    for (auto &row : cost_matrix_) row.resize(n, inf);
 
    cost_matrix_.emplace_back(n, inf);

    for (size_t i = 0; i < n; ++i) cost_matrix_[i][i] = 0;
}

void Graph::set_matrix_cost(VertexId u, VertexId v, int c) {
    if (u >= cost_matrix_.size() || v >= cost_matrix_.size()) return;
    cost_matrix_[u][v] = c;
}

const std::vector<std::vector<int>>& Graph::cost_matrix() const {
    return cost_matrix_;
}

std::optional<Graph::VertexId> Graph::vertex_id_by_label(const std::string& label) const {
    for (const auto &kv : vertices_) {
        const auto *v = kv.second.get();
        if (v && v->label() == label) return kv.first;
    }
    return std::nullopt;
}
