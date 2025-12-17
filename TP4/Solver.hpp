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
    Solver(Graph<Vertex> &_g)
        : g(_g)
    {
        auto _ = g.vertices();
        vertices = std::vector<Vertex>(_.begin(), _.end());
    }

    ~Solver()
    {
    }

    bool solve()
    {
        /**/ std::cout << "Solving" << std::endl; /**/
        solve_greedy();

        return true; // placeholder
    }

    void solve_greedy()
    {
        /**/ std::cout << "Solving greedy" << std::endl; /**/
        std::vector<Vertex> queue = vertices;
        std::sort(queue.begin(), queue.end(),
                [this](Vertex& a, Vertex& b) -> int {
                    return g.degree(a) - g.degree(b);
                });
        
        while(!queue.empty()) {
            Vertex& v = queue.front();

            independant.insert(v);
            remove_neighbors_from_queue(queue, v);
        }
    }

    bool improve()
    {
        Vertex randomVertex = pickRandomVertex();
        Graph subGraph = g.subGraph(
            g.bfs(randomVertex, SUB_GRAPH_MAX_SIZE)
        );
        SubSolver solver(subGraph);

        trimGraph(solver);
        removeSubIndependantIn(solver);

        solver.solve();

        addSubIndependantFrom(solver);

        return true; // placeholder
    }

    const std::unordered_set<Vertex>& solution() const
    {
        return independant;
    } 

private:
    void remove_neighbors_from_queue(std::vector<Vertex>& list, Vertex& v)
    {
        const auto& neighbors = g.closedNeighbors(v);
        std::erase_if(list,
            [&](const Vertex& n) {
                return neighbors.contains(n);
            });
    }

    Vertex pickRandomVertex() const
    {
        return random_element(vertices);
    }

    void trimGraph(SubSolver<Vertex>& solver)
    {
        // Tricky loop ; deletes its own elements
        std::erase_if(g.adj, [&](const auto& entry) {
            const auto& [v, ns] = entry;

            for (const Vertex n : ns) {
                if (!g.contains(n) && independant.contains(n)) {
                    return true;   // erase v
                }
            }
            return false;
        });
    }

    void removeSubIndependantIn(SubSolver<Vertex>& solver)
    {
        std::erase_if(independant, [&](const Vertex& v) {
            return solver.g.containsVertex(v);
        });
    }

    void addSubIndependantFrom(SubSolver<Vertex>& solver)
    {
        independant.insert(solver.independant.begin(), solver.independant.end());
    }
};

// Save the solution to a file
template <class Vertex>
void save(const std::string &outfn, std::unordered_set<Vertex> solution)
{
    std::ofstream outfile(outfn);
    for (Vertex v : solution)
    {
        outfile << v << std::endl;
    }
    std::cout << "Saved a dominating set of size " << solution.size() << std::endl;
}