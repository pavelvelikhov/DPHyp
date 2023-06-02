#include "join_order_search.hpp"
#include <iostream>

using namespace dphyp;

template <int N> void Solver<N>::printBitset(const std::bitset<N>& s, int n)
{
    std::cout << "{";
     for (int i=1;i<=n;i++)
        if (s[i])
            std::cout << i << ",";
        
    std::cout <<"}" << std::endl;
}

// We need to compute the neighbors of a hypernode V (set of nodes) that excludes X
// Compared to DPccp, we have a much more difficult problem here though:
//  - we use hypernodes, but we need a node-based representation of it
//  - so we pick the "smallest" node within a hypernode to represent the hypernode
//  - when considering a hyperedge (u,v), we want to add min(v) to the neighborhood
//  - but we also need to make sure the other elements of v are added to the emmited set
//  - 
template<int N> std::bitset<N> Solver<N>::neighbors(const std::bitset<N>& S, const std::bitset<N>& X)
{
    std::bitset<N> res;

    // First we need to take care of simple edges, we just add them to the neighborhood
    for (int i=1;i<=graph.nEdges;i++)
        if (S[i])
        {
            for (HyperEdge & e : graph.findSimpleEdges(i))
                if (! X[*e.to.begin()])
                    res.set(*e.to.begin());
        }

    // Now we fetch edges that result from real hyper-edges
    std::bitset<N> F = S | res | X;

    // We iterate over all edges of the graph
    // TODO: make this more efficient by building and using an index
    for (const HyperEdge & e : graph.edges){
        // if the 'from' hypernode is a subset of S and doesn't intersect with forbidden nodes,
        // add its mininum element to the result
        if (isSubset(e.from,S) && emptyIntersection(e.to,F))
        {
            int min_el = *e.from.begin();
            res.set(min_el);
            for (int el: e.from)
                F.set(el);
        }
        // if the 'to' hypernode is a subset of S and doesn't intersect with forbidden nodes,
        // add its minimum element to the result
        if (isSubset(e.to,S) && emptyIntersection(e.from,F))
        {
            int min_el = *e.to.begin();
            res.set(min_el);
            for (int el: e.to)
                F.set(el);
        }
    }

    return res;
}

template<int N> bool Solver<N>::isSubset(const std::set<int>& s1, const std::bitset<N>& s2)
{
    for(int el:s1)
        if (!s2[el])
            return false;
    return true;
}

template<int N> bool Solver<N>::emptyIntersection(const std::set<int>& s1, const std::bitset<N>& s2)
{
    for (int el:s1)
        if (s2[el])
            return false;
    return true;
}

template<int N> bool Solver<N>::solve()
{
    // Fill in the dpTable with single relation plans
    for (int i=1;i<=nNodes; i++){
        std::bitset<N> s;
        s.set(i);
        dpTable[s] = true;
        std::cout << i << std::endl;
    }

    // Process and expand singleton sets
    for (int i=nNodes;i>0;i--){
        std::bitset<N> s;
        s.set(i);
        EmitCsg(s);

        std::bitset<N> Bv;
        for (int j=i;j>0;j--)
            Bv.set(j);

        EnumerateCsgRec(s, Bv);        
    }
    
    // Return the entry of the dpTable that corresponds to the full
    // set of nodes in the graph
    std::bitset<N> V;
    for (int i=1;i<=nNodes;i++)
        V.set(i);

    return dpTable[V]; 
}

 template <int N> void Solver<N>::EmitCsg(const std::bitset<N>& S)
 {
    std::bitset<N> Bmin = computeBmin(S);
    std::bitset<N> X = S | Bmin;
    std::bitset<N> Ns = neighbors(S, X);
    for (int i=nNodes;i>0;i--)
        if (Ns[i])
        {
            std::bitset<N> S2;
            S2.set(i);
            if (containsEdge(S,S2))
                EmitCsgCmp(S,S2);
            EnumerateCmpRec(S, S2, X);
        }   
 }

 template <int N> void Solver<N>::EnumerateCsgRec(const std::bitset<N>& S, const std::bitset<N>& X)
 {
    std::bitset<N> Ns = neighbors(S,X);
    std::bitset<N> prev;
    std::bitset<N> next;

    std::cout << "Enumerate csg:" << std::endl;
    std::cout << "S:";
    printBitset(S,graph.nEdges);
    std::cout << "X:";
    printBitset(X,graph.nEdges);
    std::cout << "N:";
    printBitset(Ns,graph.nEdges);

    while(true)
    {
        next = nextBitset(prev,Ns);
        if (dpTable.contains(S | next ))
            EmitCsg(S | next );
        if (next == Ns)
            break;
    }
        
    prev.reset();
    while(true)
    {
        next = nextBitset(prev,Ns);
        EnumerateCsgRec(S | next, X | Ns );
        if (next==Ns)
            break;
    }
 }

 template <int N> void Solver<N>::EnumerateCmpRec(const std::bitset<N>& S1, const std::bitset<N>& S2, const std::bitset<N>& X)
 {
    std::bitset<N> Ns = neighbors(S2,X);

    std::bitset<N> prev;
    std::bitset<N> next;

    while(true)
    {
        next = nextBitset(prev,Ns);        
        if (dpTable.contains(S2 | next))
            if (containsEdge(S1,S2|next))
                EmitCsgCmp(S1,S2|next);
        if (next==Ns)
            break;
    }
    std::bitset<N> X2 = X | Ns;
    Ns = neighbors(S2,X2);
    prev.reset();
    while(true)
    {
        next = nextBitset(prev,Ns);        
        EnumerateCmpRec(S1,S2|next,X2);
        if (next==Ns)
            break;
    }
 }

template <int N> void Solver<N>::EmitCsgCmp(const std::bitset<N>& S1, const std::bitset<N>& S2)
{
    // Here we actually build the join and choose and compare the
    // new plan to what's in the dpTable, if it there
    // In this implementation we don't have a cost function, so we
    // just fill in the dpTable
    std::bitset<N> joined = S1 | S2;

    if (dpTable.contains(joined))
    {
        std::cout << "Duplicate entry in dpTable" << std::endl;
        exit(-1);
    }

    dpTable[joined] = true;
    std::cout << "Emitting:";
    printBitset(joined,graph.nEdges);
}

template <int N> std::bitset<N> Solver<N>::computeBmin(const std::bitset<N>& S)
{
    std::bitset<N> res;

    for (int i=1;i<=nNodes; i++)
        if (S[i])
        {
            for (int j=1;j<i;j++)
                res.set(j);
            break;
        }
    return res;
}

template <int N> std::bitset<N> Solver<N>::nextBitset(const std::bitset<N>& prev, const std::bitset<N>& final)
{
    if (prev==final)
        return final;

    return std::bitset<N>((prev | ~final).to_ulong() + 1) & final;
}

template <int N> bool Solver<N>::containsEdge(const std::bitset<N>& S, const std::bitset<N>& T)
{
    for (const HyperEdge& e: graph.edges)
    {
        bool S_subset_e = true;
        for (int i=1;i<=graph.nEdges;i++)
            if (S[i] and e.from.find(i)==e.from.end())
                S_subset_e = false;

        bool T_subset_e = true;
        for (int i=1;i<=graph.nEdges;i++)
            if (T[i] and e.to.find(i)==e.to.end())
                T_subset_e = false;

        if (S_subset_e && T_subset_e)
            return true;
    }

    return false;
}

template class dphyp::Solver<64>;