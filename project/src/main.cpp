#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <optional>
#include <iomanip>

#include "graph.hpp"
#include "io.hpp"
#include "bellman_ford.hpp"
#include "floyd.hpp"

namespace {

void print_cost_matrix(const Graph &g) {
    const auto &cm = g.cost_matrix();
    if (cm.empty()) {
        std::cout << "Matriz de custos vazia (grafo nao carregado).\n";
        return;
    }
    std::cout << "Matriz de custos (INF = " << Graph::INF << "):\n";
    for (const auto &row : cm) {
        for (int val : row) {
            if (val >= Graph::INF / 10) {
                std::cout << std::setw(5) << "INF";
            } else {
                std::cout << std::setw(5) << val;
            }
        }
        std::cout << "\n";
    }
}

void run_bellman_cli(const Graph &g) {
    if (g.num_vertices() == 0) {
        std::cout << "Grafo nao carregado. Leia uma instancia primeiro.\n";
        return;
    }
    std::cout << "Vertice fonte para Bellman-Ford: ";
    Graph::VertexId source;
    std::cin >> source;
    if (!g.has_vertex(source)) {
        std::cout << "Vertice invalido.\n";
        return;
    }
    BellmanFordResult res = bellman_ford_recursive(g, source);
    if (res.has_negative_cycle) {
        std::cout << "Ciclo negativo detectado!\n";
        return;
    }
    std::cout << "Caminho minimo de " << source << ":\n";
    for (size_t i = 0; i < res.distances.size(); ++i) {
        std::cout << "para " << i << ": ";
        if (res.distances[i] >= Graph::INF / 10) {
            std::cout << "INF";
        } else {
            std::cout << res.distances[i];
        }
        if (res.predecessors[i] != static_cast<Graph::VertexId>(-1)) {
            std::cout << " (pred: " << res.predecessors[i] << ")";
        }
        std::cout << "\n";
    }
}

void run_floyd_cli(const Graph &g) {
    if (g.num_vertices() == 0) {
        std::cout << "Grafo nao carregado. Leia uma instancia primeiro.\n";
        return;
    }
    FloydResult fres = floyd_warshall(g);
    if (fres.has_negative_cycle) {
        std::cout << "Ciclo negativo detectado!\n";
        return;
    }

    std::cout << "Matriz de Distancias:\n";
    for (size_t i = 0; i < fres.dist.size(); ++i) {
        for (size_t j = 0; j < fres.dist[i].size(); ++j) {
            if (fres.dist[i][j] >= static_cast<long long>(Graph::INF) / 10) {
                std::cout << std::setw(5) << "INF";
            } else {
                std::cout << std::setw(5) << fres.dist[i][j];
            }
        }
        std::cout << "\n";
    }

    std::cout << "\nMatriz de Antecessores:\n";
    for (size_t i = 0; i < fres.pred.size(); ++i) {
        for (size_t j = 0; j < fres.pred[i].size(); ++j) {
            if (fres.pred[i][j] == static_cast<Graph::VertexId>(-1)) {
                std::cout << std::setw(5) << "NIL";
            } else {
                std::cout << std::setw(5) << fres.pred[i][j];
            }
        }
        std::cout << "\n";
    }
}

void print_out_in_for_vertex(const Graph &g) {
    if (g.num_vertices() == 0) {
        std::cout << "Grafo nao carregado. Leia uma instancia primeiro.\n";
        return;
    }
    std::cout << "Id do vertice: ";
    Graph::VertexId v;
    std::cin >> v;
    if (!g.has_vertex(v)) {
        std::cout << "Vertice invalido.\n";
        return;
    }

    std::cout << "Outgoing (arestas saindo de " << v << "):\n";
    auto out_edges = g.outgoing(v);
    if (out_edges.empty()) {
        std::cout << "  (nenhuma)\n";
    } else {
        for (auto eid : out_edges) {
            const Edge *e = g.get_edge(eid);
            if (e) {
                std::cout << "  e" << e->id() << ": " << e->from() << " -> " << e->to()
                          << ", custo = " << e->cost() << "\n";
            }
        }
    }

    std::cout << "Incoming (arestas chegando em " << v << "):\n";
    auto in_edges = g.incoming(v);
    if (in_edges.empty()) {
        std::cout << "  (nenhuma)\n";
    } else {
        for (auto eid : in_edges) {
            const Edge *e = g.get_edge(eid);
            if (e) {
                std::cout << "  e" << e->id() << ": " << e->from() << " -> " << e->to()
                          << ", custo = " << e->cost() << "\n";
            }
        }
    }
}

} // namespace

int main(int argc, char* argv[]) {
    std::string inputPath;
    std::string dotPath;
    bool runBellman = false;
    bool runFloyd = false;
    Graph::VertexId source = 0;

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--source") == 0 && i + 1 < argc) {
            source = static_cast<Graph::VertexId>(std::stoul(argv[++i]));
        } else if (std::strcmp(argv[i], "--dot") == 0 && i + 1 < argc) {
            dotPath = argv[++i];
        } else if (std::strcmp(argv[i], "--bellman") == 0) {
            runBellman = true;
        } else if (std::strcmp(argv[i], "--floyd") == 0) {
            runFloyd = true;
        }
    }

    // Modo interativo se nenhuma flag for passada
    if (argc == 1) {
        Graph g;
        bool running = true;
        while (running) {
            std::cout << "\n===== MENU =====\n";
            std::cout << "1) Ler instancia de arquivo\n";
            std::cout << "2) Imprimir cost_matrix\n";
            std::cout << "3) Executar Bellman-Ford\n";
            std::cout << "4) Executar Floyd-Warshall\n";
            std::cout << "5) Imprimir outgoing/incoming de um vertice\n";
            std::cout << "0) Sair\n";
            std::cout << "Opcao: ";

            int opcao;
            if (!(std::cin >> opcao)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Entrada invalida. Tente novamente.\n";
                continue;
            }

            switch (opcao) {
            case 1: {
                std::cout << "Caminho do arquivo de instancia: ";
                std::string path;
                std::cin >> path;
                g = load_graph_from_file(path);
                std::cout << "Grafo carregado com " << g.num_vertices() << " vertices.\n";
                break;
            }
            case 2:
                print_cost_matrix(g);
                break;
            case 3:
                run_bellman_cli(g);
                break;
            case 4:
                run_floyd_cli(g);
                break;
            case 5:
                print_out_in_for_vertex(g);
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << "Opcao invalida.\n";
                break;
            }
        }
        return 0;
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
            std::cout << "Matriz de Distancias:\n";
            for (size_t i = 0; i < fres.dist.size(); ++i) {
                for (size_t j = 0; j < fres.dist[i].size(); ++j) {
                    if (fres.dist[i][j] >= 100000000) std::cout << std::setw(5) << "INF";
                    else std::cout << std::setw(5) << fres.dist[i][j];
                }
                std::cout << "\n";
            }

            std::cout << "\nMatriz de Antecessores:\n";
            for (size_t i = 0; i < fres.pred.size(); ++i) {
                for (size_t j = 0; j < fres.pred[i].size(); ++j) {
                    if (fres.pred[i][j] == static_cast<Graph::VertexId>(-1)) std::cout << std::setw(5) << "NIL";
                    else std::cout << std::setw(5) << fres.pred[i][j];
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
