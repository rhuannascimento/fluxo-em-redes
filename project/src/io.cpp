#include "io.hpp"

#include <fstream>
#include <sstream>
#include <unordered_map>

Graph load_graph_from_file(const std::string& path) {
    Graph g;
    std::ifstream in(path);
    if (!in.is_open()) {
        return g;
    }

    std::unordered_map<Graph::VertexId, Graph::VertexId> idmap;
    auto map_id = [&](Graph::VertexId raw) -> Graph::VertexId {
        auto it = idmap.find(raw);
        if (it != idmap.end()) return it->second;
        auto vid = g.add_vertex(std::to_string(raw));
        idmap.emplace(raw, vid);
        return vid;
    };

    std::string line;
    std::getline(in, line);
    
    if(in.fail() && line.empty()) return g;

    bool is_capacited = false;
    if (line.find("CAPACITED") != std::string::npos) {
        is_capacited = true;
    } 
    
    auto parse_line = [&](const std::string& l) {
        if (l.empty()) return;
        std::istringstream ss(l);
        Graph::VertexId u, v;
        int val = 0;
        if (!(ss >> u >> v >> val)) return;
        
        auto uid = map_id(u);
        auto vid = map_id(v);
        
        if (is_capacited) {
            g.add_edge(uid, vid, 0, val);
        } else {
            g.add_edge(uid, vid, val, 0);
        }
    };

    if (!is_capacited) {
        parse_line(line);
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        parse_line(line);
    }
    return g;
}

void write_dot(const std::string& path, const Graph& g) {
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << "digraph G {\n";
    out << "  rankdir=LR;\n";
    for (auto vid : g.vertices()) {
        const Vertex* v = g.get_vertex(vid);
        std::string label = v ? v->label() : std::to_string(vid);
        out << "  " << vid << " [label=\"" << label << "\"];\n";
    }
    for (auto eid : g.edges()) {
        const Edge* e = g.get_edge(eid);
        if (!e) continue;
        out << "  " << e->from() << " -> " << e->to()
            << " [label=\"w=" << e->cost() << "\"];\n";
    }
    out << "}\n";
}

void write_dot_capacitated(const std::string& path, const Graph& g) {
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << "digraph G {\n";
    out << "  rankdir=LR;\n";
    for (auto vid : g.vertices()) {
        const Vertex* v = g.get_vertex(vid);
        std::string label = v ? v->label() : std::to_string(vid);
        out << "  " << vid << " [label=\"" << label << "\"];\n";
    }
    for (auto eid : g.edges()) {
        const Edge* e = g.get_edge(eid);
        if (!e) continue;
        out << "  " << e->from() << " -> " << e->to()
            << " [label=\"cap=" << e->capacity() << "\"];\n";
    }
    out << "}\n";
}

void write_dot_with_flow(const std::string& path, const Graph& g) {
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << "digraph G {\n";
    out << "  rankdir=LR;\n";
    for (auto vid : g.vertices()) {
        const Vertex* v = g.get_vertex(vid);
        std::string label = v ? v->label() : std::to_string(vid);
        out << "  " << vid << " [label=\"" << label << "\"];\n";
    }
    for (auto eid : g.edges()) {
        const Edge* e = g.get_edge(eid);
        if (!e) continue;
        out << "  " << e->from() << " -> " << e->to()
            << " [label=\"" << e->flow() << "/" << e->capacity() << "\"];\n";
    }
    out << "}\n";
}
