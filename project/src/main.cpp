#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "graph.hpp"
#include "io.hpp"
#include "bellman_ford.hpp"

int main(int argc, char* argv[]) {
    std::string inputPath;
    std::string outputPath;
    std::string dotPath;
    bool runBellman = false;
    Graph::VertexId source = 0;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--output") == 0 && i + 1 < argc) {
            outputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--source") == 0 && i + 1 < argc) {
            source = static_cast<Graph::VertexId>(std::stoul(argv[++i]));
        } else if (std::strcmp(argv[i], "--dot") == 0 && i + 1 < argc) {
            dotPath = argv[++i];
        } else if (std::strcmp(argv[i], "--bellman") == 0) {
            runBellman = true;
        }
    }

    if (inputPath.empty()) {
        std::cout << "Usage: app --input <path> [--output <path>] --source <s>\n";
        return 2;
    }

    Graph g = load_graph_from_file(inputPath);

    if (runBellman) {
        BellmanFordResult res = bellman_ford_recursive(g, source);
        if (res.has_negative_cycle) {
            std::cout << "Ciclo negativo detectado!\n";
        } else {
            std::cout << "Caminho minimo de " << source << ":\n";
            for (size_t i = 0; i < res.distances.size(); ++i) {
                std::cout << "para " << i << ": ";
                if (res.distances[i] >= 1e8) std::cout << "INF";
                else std::cout << res.distances[i];
                
                if (res.predecessors[i] != static_cast<Graph::VertexId>(-1)) {
                    std::cout << " (pred: " << res.predecessors[i] << ")";
                }

                std::cout << "\n";
            }
        }
    }

    if (!dotPath.empty()) {
        write_dot(dotPath, g);
    } else {
        write_dot("graph.dot", g);
    }

    return 0;
}
