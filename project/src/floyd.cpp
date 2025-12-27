#include "floyd.hpp"

#include <limits>

FloydResult floyd_warshall(const Graph& g) {
    FloydResult result;
    const auto& costMatrix = g.cost_matrix();
    size_t vertexCount = costMatrix.size();
    result.dist.assign(vertexCount, std::vector<long long>(vertexCount, g.INF));
    const long long INF_LL = static_cast<long long>(g.INF);

    for (size_t src = 0; src < vertexCount; ++src) {
        for (size_t dst = 0; dst < vertexCount; ++dst) {
            result.dist[src][dst] = static_cast<long long>(costMatrix[src][dst]);
        }
    }

    for (size_t mid = 0; mid < vertexCount; ++mid) {
        for (size_t src = 0; src < vertexCount; ++src) {
            if (result.dist[src][mid] >= INF_LL) continue;
            for (size_t dst = 0; dst < vertexCount; ++dst) {
                if (result.dist[mid][dst] >= INF_LL) continue;
                long long candidate = result.dist[src][mid] + result.dist[mid][dst];
                if (candidate < result.dist[src][dst]) result.dist[src][dst] = candidate;
            }
        }
    }

    for (size_t v = 0; v < vertexCount; ++v) {
        if (result.dist[v][v] < 0) {
            result.has_negative_cycle = true;
            break;
        }
    }

    return result;
}
