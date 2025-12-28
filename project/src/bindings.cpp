#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "graph.hpp"
#include "vertex.hpp"
#include "edge.hpp"
#include "io.hpp"

namespace py = pybind11;

PYBIND11_MODULE(graph_native, m) {
    m.doc() = "Native graph module implemented in C++";

    py::class_<Vertex>(m, "Vertex")
        .def_property_readonly("id", &Vertex::id)
        .def_property_readonly("label", &Vertex::label);

    py::class_<Edge>(m, "Edge")
        .def_property_readonly("id", &Edge::id)
        .def_property_readonly("from_node", &Edge::from) 
        .def_property_readonly("from", &Edge::from)
        .def_property_readonly("to", &Edge::to)
        .def_property_readonly("cost", &Edge::cost);

    py::class_<Graph>(m, "Graph")
        .def(py::init<>())
        .def("outgoing", &Graph::outgoing)
        .def("incoming", &Graph::incoming)
        .def("edge_cost", &Graph::edge_cost)
        .def("has_vertex", &Graph::has_vertex)
        .def("has_edge", &Graph::has_edge)
        .def("vertices", &Graph::vertices)
        .def("edges", &Graph::edges)
        .def("get_vertex", &Graph::get_vertex, py::return_value_policy::reference_internal)
        .def("get_edge", &Graph::get_edge, py::return_value_policy::reference_internal)
        .def("add_vertex", &Graph::add_vertex, py::arg("label") = "")
        .def("add_edge", &Graph::add_edge, py::arg("from"), py::arg("to"), py::arg("cost") = 0)
        .def("get_full_graph_structure", [](const Graph& g) {
            std::unordered_map<int, std::vector<std::unordered_map<std::string, int>>> structure;
            for (auto v_id : g.vertices()) {
                std::vector<std::unordered_map<std::string, int>> neighbors;
                for (auto e_id : g.outgoing(v_id)) {
                    const auto* edge = g.get_edge(e_id);
                    if (edge) {
                        neighbors.push_back({{"to", static_cast<int>(edge->to())}, {"cost", edge->cost()}});
                    }
                }
                structure[v_id] = neighbors;
            }
            return structure;
        }, "Returns the full graph structure as a dictionary of adjacency lists.")
        .def("get_graph_statistics", [](const Graph& g) {
            std::unordered_map<std::string, py::object> stats;
            stats["num_vertices"] = py::int_(g.vertices().size());
            stats["num_edges"] = py::int_(g.edges().size());
            
            bool has_negative = false;
            int min_cost = 1000000000;
            int max_cost = -1000000000;
            
            auto edges = g.edges();
            if (!edges.empty()) {
                min_cost = g.edge_cost(edges[0]);
                max_cost = min_cost;
            }

            for (auto e_id : edges) {
                int c = g.edge_cost(e_id);
                if (c < 0) has_negative = true;
                if (c < min_cost) min_cost = c;
                if (c > max_cost) max_cost = c;
            }
            
            stats["has_negative_edges"] = py::bool_(has_negative);
            if (!edges.empty()) {
                stats["min_edge_cost"] = py::int_(min_cost);
                stats["max_edge_cost"] = py::int_(max_cost);
            } else {
                stats["min_edge_cost"] = py::none();
                stats["max_edge_cost"] = py::none();
            }
            
            return stats;
        }, "Returns statistics about the graph.");

    m.def("load_graph_from_file", [](const std::string& path) {
        return std::make_unique<Graph>(load_graph_from_file(path));
    }, "Load a graph from a file");
}
