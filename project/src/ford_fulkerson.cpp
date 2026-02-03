#include "ford_fulkerson.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include <map>
#include <chrono>

using namespace std;

namespace {

bool dfs(int u, int t, const vector<vector<int>>& adj, 
         const vector<vector<long long>>& capacity, 
         vector<vector<long long>>& flow, 
         vector<int>& parent, 
         vector<bool>& visited) {
    visited[u] = true;
    if (u == t) return true;

    for (int v : adj[u]) {
        if (!visited[v] && capacity[u][v] - flow[u][v] > 0) {
            parent[v] = u;
            if (dfs(v, t, adj, capacity, flow, parent, visited)) {
                return true;
            }
        }
    }
    return false;
}

} 

FordFulkersonResult ford_fulkerson(const Graph& g, Graph::VertexId source, Graph::VertexId sink) {
    size_t n = g.num_vertices();
    if (n == 0 || source >= n || sink >= n) {
        return {0, Graph{}};
    }

    vector<vector<long long>> capacity(n, vector<long long>(n, 0));
    vector<vector<long long>> flow(n, vector<long long>(n, 0));
    vector<vector<int>> adj(n);

    for (const auto& eid : g.edges()) {
        const Edge* e = g.get_edge(eid);
        if (!e) continue;
        int u = e->from();
        int v = e->to();
        int cap = e->capacity();
        
        if (cap <= 0) continue; 

        bool found = false;
        for(int neighbor : adj[u]) if(neighbor == v) found = true;
        if(!found) {
            adj[u].push_back(v);
            adj[v].push_back(u); 
        }

        capacity[u][v] += cap;
    }

    long long max_flow = 0;
    vector<int> parent(n);
    vector<bool> visited(n);

    while (true) {
        fill(visited.begin(), visited.end(), false);
        fill(parent.begin(), parent.end(), -1);
        
        if (!dfs(source, sink, adj, capacity, flow, parent, visited)) {
            break;
        }

        long long path_flow = -1;
        
        int curr = sink;
        while (curr != static_cast<int>(source)) {
            int prev = parent[curr];
            long long residual = capacity[prev][curr] - flow[prev][curr];
            if (path_flow == -1 || residual < path_flow) {
                path_flow = residual;
            }
            curr = prev;
        }
        
        if (path_flow == 0) break; 

        curr = sink;
        while (curr != static_cast<int>(source)) {
            int prev = parent[curr];
            flow[prev][curr] += path_flow;
            flow[curr][prev] -= path_flow;
            curr = prev;
        }

        max_flow += path_flow;
    }
  
    Graph flow_graph;
    std::map<int, int> id_map;

    for (auto vid : g.vertices()) {
        const Vertex* v = g.get_vertex(vid);
        auto new_id = flow_graph.add_vertex(v->label()); 
        id_map[vid] = new_id;
    }

    auto remaining_flow = flow;

    for (auto eid : g.edges()) {
        const Edge* e = g.get_edge(eid);
        if (!e) continue;
        int u = e->from();
        int v = e->to();
        int cap = e->capacity();
        int cost = e->cost();
        
        long long f = remaining_flow[u][v]; 
        int assigned_flow = 0;

        if (f > 0) {
            assigned_flow = std::min((long long)cap, f);
            remaining_flow[u][v] -= assigned_flow;
        }
        
        auto new_eid = flow_graph.add_edge(id_map[u], id_map[v], cost, cap);
        flow_graph.set_edge_flow(new_eid, assigned_flow);
    }

    return {max_flow, std::move(flow_graph)};
}
