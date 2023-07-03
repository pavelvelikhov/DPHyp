#include "join_order_search.hpp"
#include <iostream>

using namespace dphyp;

void printTabs(int ntabs)
{
    for (int i=0;i<ntabs;i++)
        std::cout << "\t";
}

template <int N> void DPccpSolver<N>::printBitset(const std::bitset<N>& s, int n, std::string name, int ntabs)
{
    printTabs(ntabs);
    
    std::cout << name << ": " << "{";
     for (int i=0;i<n;i++)
        if (s[i])
            std::cout << i << ",";
        
    std::cout <<"}" << std::endl;
}

// Compute neighbors of a node
template<int N> std::bitset<N> DPccpSolver<N>::neighbors(const std::bitset<N>& S, const std::bitset<N>& X)
{
    std::bitset<N> res;

    for (int i=0;i<graph.nEdges;i++)
        if (S[i])
        {
            std::bitset<N> n = graph.findNeighbors(i);
            res = res | n;
        }

    res = res & ~ X;
    return res;
}

template<int N> bool DPccpSolver<N>::solve()
{
    // Process singleton sets
    for (int i=nNodes-1;i>=0;i--){
        std::bitset<N> s;
        s.set(i);
        dpTable[s] = true;
    }

    // Expand singleton sets
    for (int i=nNodes-1;i>=0;i--){
        std::bitset<N> s;
        s.set(i);
        EmitCsg(s);
        EnumerateCsgRec(s, makeBiMin(s));        
    }
    
    // Return the entry of the dpTable that corresponds to the full
    // set of nodes in the graph
    std::bitset<N> V;
    for (int i=1;i<=nNodes;i++)
        V.set(i);

    return dpTable[V]; 
}

 template <int N> void DPccpSolver<N>::EmitCsg(const std::bitset<N>& S,int ntabs)
 {
    std::bitset<N> X = S | makeBiMin(S);
    std::bitset<N> Ns = neighbors(S, X);
    
    printTabs(ntabs);
    std::cout << "EmitCsg:" << std::endl;
    printBitset(S,graph.nEdges,"S",ntabs);

    if (Ns==std::bitset<N>())
        return;

    for (int i=nNodes-1;i>=0;i--)
        if (Ns[i])
        {
            std::bitset<N> S2;
            S2.set(i);
            EmitCsgCmp(S,S2,ntabs+1);
            EnumerateCmpRec(S, S2, X|makeB(Ns,i), ntabs+1);
        }   
 }

 template <int N> void DPccpSolver<N>::EnumerateCsgRec(const std::bitset<N>& S, const std::bitset<N>& X, int ntabs)
 {
    std::bitset<N> Ns = neighbors(S,X);

    printTabs(ntabs);
    std::cout << "Enumerate csg rec:" << std::endl;
    printBitset(S,graph.nEdges,"S",ntabs);
    printBitset(X,graph.nEdges,"X",ntabs);
    printBitset(Ns,graph.nEdges,"N",ntabs);
    
    if (Ns == std::bitset<N>())
        return;

    std::bitset<N> prev;
    std::bitset<N> next;

    while(true)
    {
        next = nextBitset(prev,Ns);
        EmitCsg(S | next );
        if (next == Ns)
            break;
        prev = next;
    }
        
    prev.reset();
    while(true)
    {
        next = nextBitset(prev,Ns);
        EnumerateCsgRec(S | next, X | Ns , ntabs+1);
        if (next==Ns)
            break;
        prev = next;
    }
 }

 template <int N> void DPccpSolver<N>::EnumerateCmpRec(const std::bitset<N>& S1, const std::bitset<N>& S2, const std::bitset<N>& X, int ntabs)
 {
    std::bitset<N> Ns = neighbors(S2,X);

    printTabs(ntabs);
    std::cout << "Enumerate cmp rec:" << std::endl;
    printBitset(S1,graph.nEdges,"S1",ntabs);
    printBitset(S2,graph.nEdges,"S2",ntabs);
    printBitset(X,graph.nEdges,"X",ntabs);
    printBitset(Ns,graph.nEdges,"N",ntabs);

    if (Ns==std::bitset<N>())
        return;

    std::bitset<N> prev;
    std::bitset<N> next;

    while(true)
    {
        next = nextBitset(prev,Ns);        
        EmitCsgCmp(S1,S2|next,ntabs+1);
        if (next==Ns)
            break;
        prev = next;
    }

    prev.reset();
    while(true)
    {
        next = nextBitset(prev,Ns);        
        EnumerateCmpRec(S1,S2|next,X|Ns,ntabs+1);
        if (next==Ns)
            break;
        prev = next;
    }
 }

template <int N> void DPccpSolver<N>::EmitCsgCmp(const std::bitset<N>& S1, const std::bitset<N>& S2, int ntabs)
{
    // Here we actually build the join and choose and compare the
    // new plan to what's in the dpTable, if it there
    // In this implementation we don't have a cost function, so we
    // just fill in the dpTable

    if (!dpTable.contains(S1)){
        printBitset(S1,graph.nEdges,"ERROR: entry not found",ntabs);
    }

    if (!dpTable.contains(S2)){
        printBitset(S2,graph.nEdges,"ERROR: entry not found",ntabs);
    }

    std::bitset<N> joined = S1 | S2;

    printBitset(joined,graph.nEdges,"Emitting",ntabs);

    //for (auto k : dpTable )
    //{
    //    std::cout << "Hash:" << std::hash<std::bitset<N>>{}(k.first) << std::endl;
    //    std::cout << "TABLE ENTRY:";
    //    printBitset(k.first,graph.nEdges);
    //    std::cout << std::endl;
    //}

    dpTable[joined] = true;
    auto pair = std::pair<std::bitset<N>,std::bitset<N>>(S1,S2);
    if (checkTable.contains(pair)){
        std::cout << "ERROR: pair already in checktable" << std::endl;
    }

    printBitset(S1,graph.nEdges,"PAIR 1",ntabs);
    printBitset(S2,graph.nEdges,"PAIR 2",ntabs);
    
    checkTable[ std::pair<std::bitset<N>,std::bitset<N>>(S1,S2) ] = true;

}

template <int N> std::bitset<N> DPccpSolver<N>::makeBiMin(const std::bitset<N>& S)
{
    std::bitset<N> res;

    for (int i=0;i<nNodes; i++)
        if (S[i])
        {
            for (int j=0;j<=i;j++)
                res.set(j);
            break;
        }
    return res;
}

template <int N> std::bitset<N> DPccpSolver<N>::makeB(const std::bitset<N>& S, int x){
    std::bitset<N> res;

    for (int i=0;i<nNodes; i++)
        if (S[i] && i<=x)
            res.set(i);

    return res;
}

template <int N> std::bitset<N> DPccpSolver<N>::nextBitset(const std::bitset<N>& prev, const std::bitset<N>& final)
{
    if (prev==final)
        return final;

    std::bitset<N> res = prev;

    bool carry = true;
    for (int i=0; i<N; i++)
    {
        if (!carry)
            break;

        if (!final[i])
            continue;

        if (res[i]==1 && carry)
            res.reset(i);
        else if (res[i]==0 && carry)
        {
            res.set(i);
            carry = false;
        }
    }

    return res;

    // return std::bitset<N>((prev | ~final).to_ulong() + 1) & final;
}

template class dphyp::DPccpSolver<64>;
template class dphyp::Graph<64>;