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
#include "ford_fulkerson.hpp"

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

void print_capacity_matrix(const Graph &g) {
    const auto &cm = g.capacity_matrix();
    if (cm.empty()) {
        std::cout << "Matriz de capacidades vazia (grafo nao carregado).\n";
        return;
    }
    std::cout << "Matriz de Capacidades:\n";
    for (const auto &row : cm) {
        for (int val : row) {
            std::cout << std::setw(5) << val;
        }
        std::cout << "\n";
    }
}

void run_bellman_cli(const Graph &g) {
    if (g.num_vertices() == 0) {
        std::cout << "Grafo nao carregado. Leia uma instancia primeiro.\n";
        return;
    }
    std::cout << "Vertice fonte para Bellman-Ford (Label): ";
    std::string source_label;
    std::cin >> source_label;
    
    auto source_opt = g.vertex_id_by_label(source_label);
    if (!source_opt.has_value()) {
        std::cout << "Vertice invalido.\n";
        return;
    }
    Graph::VertexId source = source_opt.value();

    BellmanFordResult res = bellman_ford_recursive(g, source);
    if (res.has_negative_cycle) {
        std::cout << "Ciclo negativo detectado!\n";
        return;
    }
    std::cout << "Caminho minimo de " << source_label << " (" << source << "):\n";
    for (size_t i = 0; i < res.distances.size(); ++i) {
        std::string target_label = std::to_string(i);
        const Vertex* v = g.get_vertex(i);
        if(v) target_label = v->label();

        std::cout << "para " << target_label << ": ";
        if (res.distances[i] >= Graph::INF / 10) {
            std::cout << "INF";
        } else {
            std::cout << res.distances[i];
        }
        if (res.predecessors[i] != static_cast<Graph::VertexId>(-1)) {
            std::string pred_label = std::to_string(res.predecessors[i]);
            const Vertex* pv = g.get_vertex(res.predecessors[i]);
            if(pv) pred_label = pv->label();
            std::cout << " (pred: " << pred_label << ")";
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

void run_ford_fulkerson_cli(const Graph &g) {
    if (g.num_vertices() == 0) {
        std::cout << "Grafo nao carregado. Leia uma instancia primeiro.\n";
        return;
    }
    std::cout << "Vertice origem (source - Label): ";
    std::string source_label;
    std::cin >> source_label;
    std::cout << "Vertice destino (sink - Label): ";
    std::string sink_label;
    std::cin >> sink_label;
    
    auto source_opt = g.vertex_id_by_label(source_label);
    auto sink_opt = g.vertex_id_by_label(sink_label);

    if (!source_opt.has_value() || !sink_opt.has_value()) {
        std::cout << "Vertices invalidos (Labels nao encontrados).\n";
        return;
    }
    
    Graph::VertexId source = source_opt.value();
    Graph::VertexId sink = sink_opt.value();
    
    FordFulkersonResult res = ford_fulkerson(g, source, sink);
    std::cout << "Fluxo Maximo de " << source_label << " para " << sink_label << ": " << res.max_flow << "\n";
    
    write_dot_with_flow("flow_graph.dot", res.flow_graph);
    std::cout << "Grafo com fluxos salvo em 'flow_graph.dot'. Use 'dot -Tpng flow_graph.dot -o flow.png' para visualizar.\n";
}

void print_out_in_for_vertex(const Graph &g) {
    if (g.num_vertices() == 0) {
        std::cout << "Grafo nao carregado. Leia uma instancia primeiro.\n";
        return;
    }
    std::cout << "Label do vertice: ";
    std::string label;
    std::cin >> label;
    
    auto vid_opt = g.vertex_id_by_label(label);
    if (!vid_opt.has_value()) {
        std::cout << "Vertice nao encontrado.\n";
        return;
    }
    Graph::VertexId v = vid_opt.value();

    const auto* vertex_ptr = g.get_vertex(v);
    if(vertex_ptr) {
        std::cout << "Informacoes do Vertice: " << vertex_ptr->label() << " (ID: " << v << ")\n";
    }

    std::cout << "Outgoing (arestas saindo de " << label << "):\n";
    auto out_edges = g.outgoing(v);
    if (out_edges.empty()) {
        std::cout << "  (nenhuma)\n";
    } else {
        for (auto eid : out_edges) {
            const Edge *e = g.get_edge(eid);
            if (e) {
                std::string to_label = std::to_string(e->to());
                const Vertex* tv = g.get_vertex(e->to());
                if(tv) to_label = tv->label();

                std::cout << "  " << label << " -> " << to_label
                          << ", cap: " << e->capacity() << ", flow: " << e->flow() << "\n";
            }
        }
    }

    std::cout << "Incoming (arestas chegando em " << label << "):\n";
    auto in_edges = g.incoming(v);
    if (in_edges.empty()) {
        std::cout << "  (nenhuma)\n";
    } else {
        for (auto eid : in_edges) {
            const Edge *e = g.get_edge(eid);
            if (e) {
                std::string from_label = std::to_string(e->from());
                const Vertex* fv = g.get_vertex(e->from());
                if(fv) from_label = fv->label();

                std::cout << "  " << from_label << " -> " << label
                          << ", cap: " << e->capacity() << ", flow: " << e->flow() << "\n";
            }
        }
    }
}

}

int main(int argc, char* argv[]) {
    std::string inputPath;
    std::string dotPath;
    bool runBellman = false;
    bool runFloyd = false;
    bool runFord = false;
    std::string sourceLabel = "0";
    std::string sinkLabel = "0";

    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--input") == 0 && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (std::strcmp(argv[i], "--source") == 0 && i + 1 < argc) {
            sourceLabel = argv[++i];
        } else if (std::strcmp(argv[i], "--sink") == 0 && i + 1 < argc) {
            sinkLabel = argv[++i];
        } else if (std::strcmp(argv[i], "--dot") == 0 && i + 1 < argc) {
            dotPath = argv[++i];
        } else if (std::strcmp(argv[i], "--bellman") == 0) {
            runBellman = true;
        } else if (std::strcmp(argv[i], "--floyd") == 0) {
            runFloyd = true;
        } else if (std::strcmp(argv[i], "--ford") == 0) {
            runFord = true;
        }
    }

    if (argc == 1) {
        Graph g;
        bool running = true;
        while (running) {
            std::cout << "\n===== MENU =====\n";
            std::cout << "1) Ler instancia de arquivo\n";
            std::cout << "2) Imprimir cost_matrix\n";
            std::cout << "3) Imprimir capacity_matrix\n";
            std::cout << "4) Executar Bellman-Ford\n";
            std::cout << "5) Executar Floyd-Warshall\n";
            std::cout << "6) Executar Ford-Fulkerson (Fluxo Maximo)\n";
            std::cout << "7) Imprimir outgoing/incoming de um vertice\n";
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
                print_capacity_matrix(g);
                break;
            case 4:
                run_bellman_cli(g);
                break;
            case 5:
                run_floyd_cli(g);
                break;
            case 6:
                run_ford_fulkerson_cli(g);
                break;
            case 7:
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
    
    Graph::VertexId source = 0;
    auto sourceOpt = g.vertex_id_by_label(sourceLabel);
    if(sourceOpt) source = *sourceOpt;
    else if (!inputPath.empty()) { 
         if (sourceLabel != "0") std::cerr << "Aviso: Source '" << sourceLabel << "' nao encontrado. Usando 0.\n";
    }

    Graph::VertexId sink = 0;
    auto sinkOpt = g.vertex_id_by_label(sinkLabel);
    if(sinkOpt) sink = *sinkOpt;
    else if (!inputPath.empty() && sinkLabel != "0") {
         std::cerr << "Aviso: Sink '" << sinkLabel << "' nao encontrado. Usando 0.\n";
    }

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

    if (runFord) {
        FordFulkersonResult res = ford_fulkerson(g, source, sink);
        std::cout << "Fluxo Maximo de " << sourceLabel << " para " << sinkLabel << ": " << res.max_flow << "\n";
        
        std::string outPath = !dotPath.empty() ? dotPath : "flow_graph.dot";
        
        write_dot_with_flow(outPath, res.flow_graph);
        std::cout << "Grafo com fluxos salvo em '" << outPath << "'.\n";
    }

    if (!dotPath.empty() && !runFord) {
        write_dot(dotPath, g);
    } else if (!runFord) {
        write_dot("graph.dot", g);
    }

    return 0;
}
