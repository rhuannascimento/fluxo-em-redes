#include "bellman_ford.hpp"
#include <vector>
#include <algorithm>

const int INF = 1e9;

static std::vector<std::vector<int>> memo;
static const Graph* graph_ptr = nullptr;
static Graph::VertexId src_vertex;

static int solve(int k, Graph::VertexId v) {
    if (k == 0) {
        return (v == src_vertex) ? 0 : INF;
    }
    if (memo[k][v] != -1) {
        return memo[k][v];
    }

    int res = solve(k - 1, v);

    for (auto edge_id : graph_ptr->incoming(v)) {
        const auto* edge = graph_ptr->get_edge(edge_id);
        int u = edge->from();
        int weight = edge->cost();
        
        int prev_dist = solve(k - 1, u);
        if (prev_dist != INF) {
            if (prev_dist + weight < res) {
                res = prev_dist + weight;
            }
        }
    }

    return memo[k][v] = res;
}

static int get_predecessor_recursive(int k, Graph::VertexId v) {
    if (k == 0) return -1;
    if (solve(k, v) == INF) return -1;

    if (solve(k, v) == solve(k - 1, v)) {
        return get_predecessor_recursive(k - 1, v);
    }

    int current_dist = solve(k, v);
    for (auto edge_id : graph_ptr->incoming(v)) {
        const auto* edge = graph_ptr->get_edge(edge_id);
        int u = edge->from();
        int weight = edge->cost();
        
        int prev_dist = solve(k - 1, u);
        if (prev_dist != INF && prev_dist + weight == current_dist) {
            return u;
        }
    }
    return -1;
}

BellmanFordResult bellman_ford_recursive(const Graph& g, Graph::VertexId source) {
    graph_ptr = &g;
    src_vertex = source;
    int V = g.vertices().size();
    
    memo.assign(V + 1, std::vector<int>(V, -1));

    BellmanFordResult result;
    result.distances.resize(V);
    result.predecessors.resize(V, -1); 
    result.has_negative_cycle = false;

    for (int v = 0; v < V; ++v) {
        result.distances[v] = solve(V - 1, v);
    }

    for (int v = 0; v < V; ++v) {
        if (static_cast<Graph::VertexId>(v) != source && result.distances[v] != INF) {
            result.predecessors[v] = get_predecessor_recursive(V - 1, v);
        }
    }

    for (int v = 0; v < V; ++v) {
        int dist_V = solve(V, v);
        if (dist_V < result.distances[v]) {
            result.has_negative_cycle = true;
            break;
        }
    }

    return result;
}
