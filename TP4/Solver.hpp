#pragma once

#include <filesystem>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <algorithm>

#include "Graph.hpp"
#include "SubSolver.hpp"
#include "tools.hpp"

template <class Vertex>
using Edge = std::pair<Vertex, Vertex>;

constexpr int SUB_GRAPH_MAX_SIZE = 3000;

template <class Vertex>
class Solver
{
private:
    Graph<Vertex>& g;

    std::unordered_set<Vertex> independant;
    std::vector<Vertex> vertices, vVerticesToBeChecked;

public:
    Solver(Graph<Vertex>& _g)
        : g(_g),
          independant(),
          vertices(g.vertices().begin(), g.vertices().end()),
          vVerticesToBeChecked(vertices)
    {}

    void solve_greedy()
    {
        std::cout << "Solving greedy" << std::endl;

        std::vector<Vertex> queue = vertices;
        std::sort(queue.begin(), queue.end(),
            [this](const Vertex& a, const Vertex& b) {
                return g.degree(a) < g.degree(b);
            });

        while (!queue.empty()) {
            Vertex v = queue.front();

            independant.insert(v);

            removeNeighborsFromQueue(queue, v);

            // keep "to be checked" in sync
            auto it = std::find(vVerticesToBeChecked.begin(),
                                vVerticesToBeChecked.end(), v);
            if (it != vVerticesToBeChecked.end())
                vVerticesToBeChecked.erase(it);
        }
    }

    bool improve()
    {
        if (vVerticesToBeChecked.empty())
            return false;

        Vertex randomVertex = popRandomVertex();
        auto subVertices = g.bfs(randomVertex, SUB_GRAPH_MAX_SIZE);
        int score = countSubIndependant(subVertices);

        Graph<Vertex> subGraph = g.subGraph(subVertices);
        trimGraph(subGraph, subVertices);

        SubSolver<Vertex> solver(subGraph);
        if (!solver.solve())
            return false;

        std::unordered_set<Vertex> subSolution = solver.independant();

        if (solver.optimal()) {
            checkSubVertices(subVertices);
        }

        if (countSubIndependant(subSolution) <= score) {
            return false;
        }

        removeSubIndependantFrom(solver);
        addSubIndependant(subSolution);
        return true;
    }

    const std::unordered_set<Vertex>& solution() const
    {
        return independant;
    }

    bool solved() const
    {
        return vVerticesToBeChecked.empty();
    }

private:
    void removeNeighborsFromQueue(std::vector<Vertex>& list,
                                  const Vertex& v) const
    {
        const auto& neighbors = g.closedNeighbors(v);
        std::erase_if(list,
            [&](const Vertex& n) {
                return neighbors.contains(n);
            });
    }

    void checkSubVertices(const std::vector<Vertex>& subVertices)
    {
        for (const Vertex& v : subVertices) {
        auto it = std::find(vVerticesToBeChecked.begin(),
                            vVerticesToBeChecked.end(), v);
        if (it != vVerticesToBeChecked.end())
            vVerticesToBeChecked.erase(it);
    }
    }

    int countSubIndependant(const std::vector<Vertex>& vs) const
    {
        int count = 0;
        for (const Vertex& v : vs)
            if (independant.contains(v))
                ++count;
        return count;
    }

    int countSubIndependant(const std::unordered_set<Vertex>& vs) const
    {
        int count = 0;
        for (const Vertex& v : vs)
            if (independant.contains(v))
                ++count;
        return count;
    }

    Vertex popRandomVertex() const
    {
        if (vVerticesToBeChecked.empty())
            throw std::logic_error("popRandomVertex on empty set");

        Vertex v = random_element(vVerticesToBeChecked);
        std::erase(vVerticesToBeChecked, v);
        return v;
    }

    void trimGraph(Graph<Vertex>& subGraph,
                   const std::vector<Vertex>& subVertices)
    {
        std::unordered_set<Vertex> inSub(subVertices.begin(), subVertices.end());

        std::erase_if(subGraph.adj, [&](const auto& entry) {
            const Vertex& u = entry.first;

            for (const Vertex& n : g.neighbors(u)) {
                if (!inSub.contains(n) && independant.contains(n))
                    return true;
            }
            return false;
        });
    }

    void removeSubIndependantFrom(SubSolver<Vertex>& solver)
    {
        std::erase_if(independant, [&](const Vertex& v) {
            return solver.g.containsVertex(v);
        });
    }

    void addSubIndependant(const std::unordered_set<Vertex>& sol)
    {
        independant.insert(sol.begin(), sol.end());
    }
};

// Save the solution to a file
template <class Vertex>
void save(const std::string& outfn,
          const std::unordered_set<Vertex>& solution)
{
    std::ofstream outfile(outfn);
    for (Vertex v : solution)
        outfile << v << '\n';

    std::cout << "Saved an independant set of size "
              << solution.size() << std::endl;
}
