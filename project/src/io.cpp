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
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        Graph::VertexId u, v;
        int w = 0;
        if (!(ss >> u >> v >> w)) continue;
        
        auto uid = map_id(u);
        auto vid = map_id(v);
        g.add_edge(uid, vid, w);
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