#pragma once

#include <filesystem>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include "Graph.hpp"
#include "SubSolver.hpp"
#include "tools.hpp"

template <class Vertex>
using Edge = std::pair<Vertex, Vertex>;

constexpr int SUB_GRAPH_MAX_SIZE = 5000;

template <class Vertex>
class Solver
{
private:
    Graph<Vertex>& g;

    std::vector<Vertex> vertices;
    std::unordered_set<Vertex> independant;

public:
    Solver(Graph<Vertex> &g, int maxtime)
        : g(g),
          vertices(g.vertices())
    {
    }

    ~Solver()
    {
    }

    bool solve()
    {
        /**/ std::cout << "Solving" << std::endl; /**/
        solve_greedy();
        
    }

    void solve_greedy()
    {
        std::unordered_set<Vertex> queue(vertices.begin(), vertices.end());
        std::sort(queue.begin(), queue.end(),
                [](Vertex& a, Vertex& b) -> int {
                    return a.degree() - b.degree();
                });
        
        while(!queue.empty()) {
            Vertex& v = queue.front();

            independant.insert(v);
            remove_neighbors_from_queue(queue, v);
        }
    }

    void improve()
    {
        Vertex randomVertex = pickRandomVertex();
        Graph subGraph = g.subGraph(
            g.bfs(randomVertex, SUB_GRAPH_MAX_SIZE);
        );
        SubSolver solver(subGraph);

        trimGraph(solver);

        solver.solve();
    }

    void remove_neighbors_from_queue(std::vector<Vertex>& list, Vertex& v)
    {
        std::erase(
            std::remove_if(list,
                [&v](Vertex& n) { return g.closedneighbors(v).contains(n); }
            ),
            list.end()
        );
    }

    Vertex pickRandomVertex() const
    {
        return random_element(vertices());
    }

    void trimGraph(SubSolver& solver)
    {
        
    }

    void isOutsideAndIndependant(Graph& subGraph, Vertex v)
    {
        return !subGraph.contains(v) && independant.contains(v)
    }

    void save(std::string fn) const
    {
        /**/ std::cout << "Saving solution" << std::endl; /**/

        if (!cplex.isPrimalFeasible()) {
            std::cout << "No feasible solution to save (status: " << cplex.getCplexStatus() << ")" << std::endl;
            return;
        }

        std::cout << "Solution is " << cplex.getCplexStatus() << std::endl;

        solution.clear();
        for (Vertex v : g.vertices()) {
            if (cplex.getValue(variables[v]) > 0.5)
                solution.insert(v);
        }

        std::ofstream outfile(fn);
        for (Vertex v : solution) {
            outfile << v << std::endl;
        }
        std::cout << "Saved an independent set of size " << solution.size() << std::endl;
    }
};
