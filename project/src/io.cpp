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
        int c;
        if (!(ss >> u >> v >> c)) continue;
        auto uid = map_id(u);
        auto vid = map_id(v);
        g.add_edge(uid, vid, c);
    }
    return g;
}

FlowReport build_flow_report(const Graph& g) {
    FlowReport r{0, {}};
    for (auto eid : g.edges()) {
        const Edge* e = g.get_edge(eid);
        if (!e) continue;
        r.entries.push_back(FlowResultEntry{
            e->from(), e->to(), e->capacity(), e->flow(), e->residual_capacity()
        });
        r.max_flow += e->flow();
    }
    return r;
}

void write_report_to_file(const std::string& path, const FlowReport& report) {
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << "max_flow " << report.max_flow << '\n';
    for (const auto& entry : report.entries) {
        out << entry.from << ' ' << entry.to
            << " capacity=" << entry.capacity
            << " flow=" << entry.flow
            << " residual=" << entry.residual << '\n';
    }
}

void write_dot(const std::string& path, const Graph& g) {
    std::ofstream out(path);
    if (!out.is_open()) return;
    out << "digraph G {\n";
    out << "  rankdir=LR;\n";
    // vertices with labels
    for (auto vid : g.vertices()) {
        const Vertex* v = g.get_vertex(vid);
        std::string label = v ? v->label() : std::to_string(vid);
        out << "  " << vid << " [label=\"" << label << "\"];\n";
    }
    // edges with capacity/flow
    for (auto eid : g.edges()) {
        const Edge* e = g.get_edge(eid);
        if (!e) continue;
        out << "  " << e->from() << " -> " << e->to()
            << " [label=\"c=" << e->capacity() << ", f=" << e->flow() << "\"];\n";
    }
    out << "}\n";
}
