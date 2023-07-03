#pragma once

#include <set>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include <bitset>

namespace dphyp {

struct Edge{
    int from;
    int to;

    Edge(int f, int t): from(f), to(t) {}

    bool operator==(const Edge& other) const
    {
        return from==other.from && to==other.to;
    }

    struct HashFunction
    {
        size_t operator()(const Edge& e) const
        {
            return e.from + e.to;
        }
    };
};

template <int N>
struct Graph {
    std::unordered_set<Edge,Edge::HashFunction> edges;
    std::unordered_map<int,std::bitset<N>> edgeIdx;
    int nEdges;
    int nNodes;

    Graph(std::unordered_set<Edge,Edge::HashFunction> e): edges(e) {
        int maxNode = 0;

        for (const Edge& edge: edges) {
            if (!edgeIdx.contains(edge.from)) {
                edgeIdx[edge.from] = std::bitset<N>();
            }
            edgeIdx[edge.from].set(edge.to);
            if (!edgeIdx.contains(edge.to)) {
                edgeIdx[edge.to] = std::bitset<N>();
            }
            edgeIdx[edge.to].set(edge.from);

            if (edge.from>maxNode){
                maxNode = edge.from;
            }
            if (edge.to>maxNode){
                maxNode = edge.to;
            }
        }

        nEdges = edges.size();
        nNodes = maxNode+1;
    }

    std::bitset<N> findNeighbors(int fromVertex)
    {
        if (edgeIdx.contains(fromVertex)) {
            return edgeIdx[fromVertex];
        } else {
            return std::bitset<N>();
        }
    }
};

}