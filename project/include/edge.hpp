#pragma once

#include <cstddef>

class Edge {
public:
    using Id = std::size_t;
    using VertexId = std::size_t;

    Edge(Id id, VertexId from, VertexId to, int cost = 0, int capacity = 0)
        : id_(id), from_(from), to_(to), cost_(cost), capacity_(capacity) {}

    Id id() const { return id_; }
    VertexId from() const { return from_; }
    VertexId to() const { return to_; }

    int cost() const { return cost_; }
    void set_cost(int c) { cost_ = c; }

    int capacity() const { return capacity_; }
    void set_capacity(int c) { capacity_ = c; }

    int flow() const { return flow_; }
    void set_flow(int f) { flow_ = f; }

private:
    Id id_;
    VertexId from_;
    VertexId to_;
    int cost_;
    int capacity_;
    int flow_{0};
};
