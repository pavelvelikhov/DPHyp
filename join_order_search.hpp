#pragma once

#include "hyper_graph.hpp"
#include "graph.hpp"
#include <bitset>
#include <set>
#include <unordered_map>

namespace dphyp {

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;  
    }
};

template <int N>
class Solver {
    public:

    Solver(HyperGraph& g, int nNodes): graph(g), nNodes(nNodes) {}
    std::bitset<N> nextBitset(const std::bitset<N>&, const std::bitset<N>&);
    void printBitset(const std::bitset<N>& s, int n);

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
    bool containsEdge(const std::bitset<N>&, const std::bitset<N>&);
};

// Implement a separate DPccp solver for testing while fixing issues with DPHyp

template <int N>
class DPccpSolver {
    public:

    DPccpSolver(Graph<N>& g): graph(g), nNodes(g.nNodes) {}
    std::bitset<N> nextBitset(const std::bitset<N>&, const std::bitset<N>&);
    void printBitset(const std::bitset<N>& s, int n, std::string name, int ntabs=0);

    bool solve();

    std::unordered_map<std::bitset<N>,bool> dpTable;
    std::unordered_map<std::pair<std::bitset<N>,std::bitset<N>>,bool,pair_hash> checkTable;

    private:

    int nNodes;
    Graph<N>& graph;
    
    void EmitCsg(const std::bitset<N>&,int=0);
    void EnumerateCsgRec(const std::bitset<N>&,const std::bitset<N>&,int=0);
    void EmitCsgCmp(const std::bitset<N>&,const std::bitset<N>&,int=0);
    void EnumerateCmpRec(const std::bitset<N>&,const std::bitset<N>&, const std::bitset<N>&,int=0);

    std::bitset<N> neighbors(const std::bitset<N>&,const std::bitset<N>&);
    std::bitset<N> makeBiMin(const std::bitset<N>&);
    std::bitset<N> makeB(const std::bitset<N>&,int);

};

}