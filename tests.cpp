#include <set>
#include <unordered_set>
#include <iostream>
#include "hyper_graph.hpp"
#include "join_order_search.hpp"

using namespace dphyp;

void test0()
{
    std::bitset<64> s;
    s.set(2);
    s.set(4);
    s.set(7);
    std::unordered_set<HyperEdge,HyperEdge::HashFunction> edges;
    HyperGraph graph(edges);
    Solver<64> solv = Solver<64>(graph,10);

    std::bitset<64> next;

    while(true)
    {
        next = solv.nextBitset(next,s);
        solv.printBitset(next,10);
        if (next==s)
            break;
    }

}
// sample hypergraph from Figure 2 of DPHyp paper
void test1()
{

    std::set<int> s1 {1};
    std::set<int> s2 {2};
    std::set<int> s3 {3};
    std::set<int> s4 {4};
    std::set<int> s5 {5};
    std::set<int> s6 {6};
    std::set<int> s123 {1,2,3};
    std::set<int> s456 {4,5,6};

    std::unordered_set<HyperEdge,HyperEdge::HashFunction> edges
        { HyperEdge(s1,s2),
          HyperEdge(s2,s3),
          HyperEdge(s4,s5),
          HyperEdge(s5,s6),
          HyperEdge(s123,s456)
          };

    HyperGraph graph(edges);
    Solver<64> s = Solver<64>(graph,6);
    s.solve();
}

// Test from DPccp paper
void test2()
{
    std::set<int> s0 {1};
    std::set<int> s1 {2};
    std::set<int> s2 {3};
    std::set<int> s3 {4};
    std::set<int> s4 {5};

    std::unordered_set<HyperEdge,HyperEdge::HashFunction> edges
        { HyperEdge(s0,s1),
          HyperEdge(s0,s2),
          HyperEdge(s0,s3),
          HyperEdge(s1,s4),
          HyperEdge(s2,s3),
          HyperEdge(s3,s4)
          };

    HyperGraph graph(edges);
    Solver<64> s = Solver<64>(graph,6);
    s.solve();
}


int main()
{
    std::cout << "Test 0: " << std::endl;
    test0();

    std::cout << "Test 2: " << std::endl;
    test2();

}