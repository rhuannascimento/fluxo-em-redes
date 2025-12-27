#include "bellman_ford.hpp"
#include <vector>
#include <algorithm>

const int NOT_CALCULATED = -2e9;

static std::vector<std::vector<int>> min_distance_by_edge_count;
static std::vector<std::vector<int>> predecessor_by_edge_count;
static const Graph* graph_context = nullptr;
static Graph::VertexId source_vertex_id;

static int distance_with_k_edges(int num_edges, Graph::VertexId vertex_id, int INF) {
    if (num_edges == 0) {
        return (vertex_id == source_vertex_id) ? 0 : INF;
    }
    if (min_distance_by_edge_count[num_edges][vertex_id] != NOT_CALCULATED) {
        return min_distance_by_edge_count[num_edges][vertex_id];
    }

    int best_distance = distance_with_k_edges(num_edges - 1, vertex_id, INF);
    int best_predecessor = predecessor_by_edge_count[num_edges - 1][vertex_id];

    for (auto incoming_edge_id : graph_context->incoming(vertex_id)) {
        const auto* incoming_edge = graph_context->get_edge(incoming_edge_id);
        int from_vertex = incoming_edge->from();
        int edge_weight = incoming_edge->cost();
        
        int prev_distance = distance_with_k_edges(num_edges - 1, from_vertex, INF);
        if (prev_distance != INF) {
            if (prev_distance + edge_weight < best_distance) {
                best_distance = prev_distance + edge_weight;
                best_predecessor = from_vertex;
            }
        }
    }

    predecessor_by_edge_count[num_edges][vertex_id] = best_predecessor;
    return min_distance_by_edge_count[num_edges][vertex_id] = best_distance;
}


BellmanFordResult bellman_ford_recursive(const Graph& g, Graph::VertexId source) {
    graph_context = &g;
    source_vertex_id = source;
    int num_vertices = g.vertices().size();
    
    min_distance_by_edge_count.assign(num_vertices + 1, std::vector<int>(num_vertices, NOT_CALCULATED));
    predecessor_by_edge_count.assign(num_vertices + 1, std::vector<int>(num_vertices, -1));

    BellmanFordResult result;
    result.distances.resize(num_vertices);
    result.predecessors.resize(num_vertices, -1); 
    result.has_negative_cycle = false;

    for (int vertex_id = 0; vertex_id < num_vertices; ++vertex_id) {
        result.distances[vertex_id] = distance_with_k_edges(num_vertices - 1, vertex_id, g.INF);
    }

    for (int vertex_id = 0; vertex_id < num_vertices; ++vertex_id) {
        if (static_cast<Graph::VertexId>(vertex_id) != source && result.distances[vertex_id] != g.INF) {
            result.predecessors[vertex_id] = predecessor_by_edge_count[num_vertices - 1][vertex_id];
        }
    }

    for (int vertex_id = 0; vertex_id < num_vertices; ++vertex_id) {
        int dist_with_num_vertices = distance_with_k_edges(num_vertices, vertex_id, g.INF);
        if (dist_with_num_vertices < result.distances[vertex_id]) {
            result.has_negative_cycle = true;
            break;
        }
    }

    return result;
}
