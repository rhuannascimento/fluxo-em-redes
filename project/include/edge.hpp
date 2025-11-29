#pragma once

#include <cstddef>

class Edge {
public:
    using Id = std::size_t;
    using VertexId = std::size_t;

    Edge(Id id, VertexId from, VertexId to, int capacity = 0, int flow = 0)
        : id_(id), from_(from), to_(to), capacity_(capacity), flow_(flow) {}

    Id id() const { return id_; }
    VertexId from() const { return from_; }
    VertexId to() const { return to_; }

    int capacity() const { return capacity_; }
    void set_capacity(int c) { capacity_ = c; }

    int flow() const { return flow_; }
    void set_flow(int f) { flow_ = f; }

    int residual_capacity() const { return capacity_ - flow_; }

private:
    Id id_;
    VertexId from_;
    VertexId to_;
    int capacity_;
    int flow_;
};
