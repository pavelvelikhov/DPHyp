#pragma once

#include <set>
#include <unordered_set>
#include <unordered_map>
#include <list>

namespace dphyp {

struct HyperEdge{
    std::set<int> from;
    std::set<int> to;

    HyperEdge(std::set<int>& f, std::set<int>& t): from(f), to(t) {}

    bool operator==(const HyperEdge& other) const
    {
        return from==other.from && to==other.to;
    }

    struct HashFunction
    {
        size_t operator()(const HyperEdge& e) const
        {
            return *e.from.begin() + *e.to.begin();
        }
    };
};

struct HyperGraph {
    std::unordered_set<HyperEdge,HyperEdge::HashFunction> edges;
    std::unordered_set<HyperEdge,HyperEdge::HashFunction> simpleEdges;
    std::unordered_map<int,std::list<HyperEdge>> simpleEdgeIdx;
    std::unordered_map<int,std::unordered_set<HyperEdge,HyperEdge::HashFunction>> edgeIdx;
    int nEdges;

    HyperGraph(std::unordered_set<HyperEdge,HyperEdge::HashFunction> e): edges(e)
    {
        for (const HyperEdge& edge: edges)
        {
            if (edge.from.size()==1 and edge.to.size()==1)
            {
                simpleEdges.emplace(edge);
                int fromVertex = *edge.from.begin();
                if (simpleEdgeIdx.find(fromVertex)==simpleEdgeIdx.end())
                    simpleEdgeIdx[fromVertex] = std::list<HyperEdge>();

                simpleEdgeIdx[fromVertex].push_back(edge);
            }
        }
        nEdges = edges.size();
    }

    std::list<HyperEdge> findSimpleEdges(int fromVertex)
    {
        if (simpleEdgeIdx.find(fromVertex)!=simpleEdgeIdx.end())
            return simpleEdgeIdx[fromVertex];
        else
            return std::list<HyperEdge>();
    }
};

}