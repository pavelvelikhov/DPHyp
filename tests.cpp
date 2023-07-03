#include <set>
#include <unordered_set>
#include <iostream>
#include "hyper_graph.hpp"
#include "join_order_search.hpp"

using namespace dphyp;

void dphyp_test0()
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
void dphyp_test1()
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
void dphyp_test2()
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
          HyperEdge(s2,s4),
          HyperEdge(s3,s4)
          };

    HyperGraph graph(edges);
    Solver<64> s = Solver<64>(graph,5);
    s.solve();
}

void dpccp_test0()
{
    std::unordered_set<Edge,Edge::HashFunction> edges
        { Edge(0,1),
          Edge(1,2),
          Edge(2,3),
          Edge(3,0)
        };

    Graph<64> graph(edges);
    DPccpSolver<64> s = DPccpSolver<64>(graph);
    s.solve();

    std::cout << "DP table size: " << s.dpTable.size() << std::endl;
    std::cout << "Check table size: " << s.checkTable.size() << std::endl;

}

void dpccp_test1()
{
    std::unordered_set<Edge,Edge::HashFunction> edges
        { Edge(0,1),
          Edge(0,2),
          Edge(0,3),
          Edge(1,4),
          Edge(2,3),
          Edge(2,4),
          Edge(3,4)
        };

    Graph<64> graph(edges);
    DPccpSolver<64> s = DPccpSolver<64>(graph);
    s.solve();

    std::cout << "DP table size: " << s.dpTable.size() << std::endl;
    std::cout << "Check table size: " << s.checkTable.size() << std::endl;

}

int main()
{
    std::cout << "DPccp test 0: " << std::endl;
    dpccp_test0();

    std::cout << "DPccp test 1: " << std::endl;
    dpccp_test1();

}