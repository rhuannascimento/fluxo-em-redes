#pragma once

#include <cstddef>
#include <string>

class Vertex {
public:
    using Id = std::size_t;

    explicit Vertex(Id id, std::string label = "")
        : id_(id), label_(std::move(label)) {}

    Id id() const { return id_; }
    const std::string& label() const { return label_; }
    void set_label(const std::string& label) { label_ = label; }

private:
    Id id_;
    std::string label_;
};
