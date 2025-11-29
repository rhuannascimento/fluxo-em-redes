#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "graph.hpp"
#include "io.hpp"

int main(int argc, char* argv[]) {
    std::string inputPath;
    std::string outputPath;
    std::string dotPath;
    Graph::VertexId source = 0;
    Graph::VertexId sink = 0;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            outputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--source") == 0 && i + 1 < argc) {
            source = static_cast<Graph::VertexId>(std::stoul(argv[++i]));
        } else if (std::strcmp(argv[i], "--sink") == 0 && i + 1 < argc) {
            sink = static_cast<Graph::VertexId>(std::stoul(argv[++i]));
        } else if (std::strcmp(argv[i], "--dot") == 0 && i + 1 < argc) {
            dotPath = argv[++i];
        }
    }

    if (inputPath.empty()) {
        std::cout << "Usage: app --input <path> [--output <path>] --source <s> --sink <t>\n";
        return 2;
    }

    Graph g = load_graph_from_file(inputPath);
    FlowReport report = build_flow_report(g);

    if (!dotPath.empty()) {
        write_dot(dotPath, g);
    } else {
        write_dot("graph.dot", g);
    }

    if (!outputPath.empty()) {
        write_report_to_file(outputPath, report);
    } else {
        std::cout << "max_flow " << report.max_flow << "\n";
        for (const auto& e : report.entries) {
            std::cout << e.from << ' ' << e.to
                      << " capacity=" << e.capacity
                      << " flow=" << e.flow
                      << " residual=" << e.residual << "\n";
        }
    }
    return 0;
}
