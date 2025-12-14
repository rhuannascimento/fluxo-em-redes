#pragma once

#include <cstddef>

class Edge {
public:
    using Id = std::size_t;
    using VertexId = std::size_t;

    Edge(Id id, VertexId from, VertexId to, int cost = 0)
        : id_(id), from_(from), to_(to), cost_(cost) {}

    Id id() const { return id_; }
    VertexId from() const { return from_; }
    VertexId to() const { return to_; }

    int cost() const { return cost_; }
    void set_cost(int c) { cost_ = c; }

private:
    Id id_;
    VertexId from_;
    VertexId to_;
    int cost_;
};
