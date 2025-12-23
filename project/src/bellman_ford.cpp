#include "bellman_ford.hpp"
#include <vector>
#include <algorithm>

const int INF = 1e9;
const int NOT_CALCULATED = -2e9;

static std::vector<std::vector<int>> min_distance_by_edge_count;
static const Graph* graph_context = nullptr;
static Graph::VertexId source_vertex_id;

static int distance_with_k_edges(int num_edges, Graph::VertexId vertex_id) {
    if (num_edges == 0) {
        return (vertex_id == source_vertex_id) ? 0 : INF;
    }
    if (min_distance_by_edge_count[num_edges][vertex_id] != NOT_CALCULATED) {
        return min_distance_by_edge_count[num_edges][vertex_id];
    }

    int best_distance = distance_with_k_edges(num_edges - 1, vertex_id);

    for (auto incoming_edge_id : graph_context->incoming(vertex_id)) {
        const auto* incoming_edge = graph_context->get_edge(incoming_edge_id);
        int from_vertex = incoming_edge->from();
        int edge_weight = incoming_edge->cost();
        
        int prev_distance = distance_with_k_edges(num_edges - 1, from_vertex);
        if (prev_distance != INF) {
            if (prev_distance + edge_weight < best_distance) {
                best_distance = prev_distance + edge_weight;
            }
        }
    }

    return min_distance_by_edge_count[num_edges][vertex_id] = best_distance;
}

static int find_predecessor_recursive(int num_edges, Graph::VertexId vertex_id) {
    if (num_edges == 0) return -1;
    if (distance_with_k_edges(num_edges, vertex_id) == INF) return -1;

    if (distance_with_k_edges(num_edges, vertex_id) == distance_with_k_edges(num_edges - 1, vertex_id)) {
        return find_predecessor_recursive(num_edges - 1, vertex_id);
    }

    int current_distance = distance_with_k_edges(num_edges, vertex_id);
    for (auto incoming_edge_id : graph_context->incoming(vertex_id)) {
        const auto* incoming_edge = graph_context->get_edge(incoming_edge_id);
        int from_vertex = incoming_edge->from();
        int edge_weight = incoming_edge->cost();
        
        int prev_distance = distance_with_k_edges(num_edges - 1, from_vertex);
        if (prev_distance != INF && prev_distance + edge_weight == current_distance) {
            return from_vertex;
        }
    }
    return -1;
}

BellmanFordResult bellman_ford_recursive(const Graph& g, Graph::VertexId source) {
    graph_context = &g;
    source_vertex_id = source;
    int num_vertices = g.vertices().size();
    
    min_distance_by_edge_count.assign(num_vertices + 1, std::vector<int>(num_vertices, NOT_CALCULATED));

    BellmanFordResult result;
    result.distances.resize(num_vertices);
    result.predecessors.resize(num_vertices, -1); 
    result.has_negative_cycle = false;

    for (int vertex_id = 0; vertex_id < num_vertices; ++vertex_id) {
        result.distances[vertex_id] = distance_with_k_edges(num_vertices - 1, vertex_id);
    }

    for (int vertex_id = 0; vertex_id < num_vertices; ++vertex_id) {
        if (static_cast<Graph::VertexId>(vertex_id) != source && result.distances[vertex_id] != INF) {
            result.predecessors[vertex_id] = find_predecessor_recursive(num_vertices - 1, vertex_id);
        }
    }

    for (int vertex_id = 0; vertex_id < num_vertices; ++vertex_id) {
        int dist_with_num_vertices = distance_with_k_edges(num_vertices, vertex_id);
        if (dist_with_num_vertices < result.distances[vertex_id]) {
            result.has_negative_cycle = true;
            break;
        }
    }

    return result;
}
