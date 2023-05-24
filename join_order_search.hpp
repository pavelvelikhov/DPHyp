#pragma once

#include "hyper_graph.hpp"
#include <bitset>
#include <set>
#include <unordered_map>

namespace dphyp {

template <int N>
class Solver {
    public:

    Solver(HyperGraph& g, int nNodes): graph(g), nNodes(nNodes) {}

    bool solve();

    private:

    int nNodes;
    HyperGraph& graph;
    std::unordered_map<std::bitset<N>,bool> dpTable;

    void EmitCsg(const std::bitset<N>&);
    void EnumerateCsgRec(const std::bitset<N>&,const std::bitset<N>&);
    void EmitCsgCmp(const std::bitset<N>&,const std::bitset<N>&);
    void EnumerateCmpRec(const std::bitset<N>&,const std::bitset<N>&, const std::bitset<N>&);

    std::bitset<N> neighbors(const std::bitset<N>&,const std::bitset<N>&);
    bool isSubset(const std::set<int>&, const std::bitset<N>&);
    bool emptyIntersection(const std::set<int>&, const std::bitset<N>&);
    std::bitset<N> computeBmin(const std::bitset<N>&);
    std::bitset<N> nextBitset(const std::bitset<N>&, const std::bitset<N>&);
    bool containsEdge(const std::bitset<N>&, const std::bitset<N>&);
};
}