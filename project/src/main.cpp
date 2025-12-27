#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "graph.hpp"
#include "io.hpp"
#include "bellman_ford.hpp"
#include "floyd.hpp"
#include <optional>
#include <iomanip>

int main(int argc, char* argv[]) {
    std::string inputPath;
    std::string outputPath;
    std::string dotPath;
    bool runBellman = false;
    bool runFloyd = false;
    std::string x1_label = "0";
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
        } else if (std::strcmp(argv[i], "--floyd") == 0) {
            runFloyd = true;
        } else if (std::strcmp(argv[i], "--x1") == 0 && i + 1 < argc) {
            x1_label = argv[++i];
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

    if (runFloyd) {
        FloydResult fres = floyd_warshall(g);
        if (fres.has_negative_cycle) {
            std::cout << "Ciclo negativo detectado!\n";
        } else {
            auto opt = g.vertex_id_by_label(x1_label);
            if (!opt) {
                std::cout << "Label X1='" << x1_label << "' nao encontrado no grafo\n";
            } else {
                auto vid = *opt;
                std::cout << "Caminho minimo de " << vid << ":\n";
                for (size_t j = 0; j < fres.dist[vid].size(); ++j) {
                    std::cout << "para " << j << ": ";
                    if (fres.dist[vid][j] >= 100000000) std::cout << "INF";
                    else std::cout << fres.dist[vid][j];
                    std::cout << "\n";
                }
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
