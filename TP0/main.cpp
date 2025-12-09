// BERTOLINI Garice

#include <iostream>
#include <cassert>
#include "graph.hpp"
#include "files.hpp"

int firstAvailableColor(const std::unordered_set<int>& colorSet)
{
    int c;
    for (c = 0; colorSet.count(c) != 0; c++)
        ;
    return c;
}

std::unordered_map<int, int> greedyColor(const Graph<int>& g)
{
    int maxColor = -1;

    std::unordered_map<int, int> color;
    std::unordered_set<int> vertices = g.vertices();

    while (todo.countVertices())
    {
        int v = todo.maxDegreeVertex();
        todo.removeVertex(v);

        std::unordered_set<int> neighborColors;

        for (int u : g.neighbors(v))
            if (color.count(u))
                neighborColors.insert(color.at(u));

        color[v] = firstAvailableColor(neighborColors);
        
        if (maxColor < color[v])
            maxColor = color[v];
    }

    std::cout << "Number of colors: " << maxColor + 1 << std::endl;
    return color;
}

void testColor(const Graph<int>& g, std::unordered_map<int, int>& color)
{
    for (int v : g.vertices())
    {
        if (color.count(v) == 0)
        {
            std::cout << "Uncolored vertex " << v << std::endl;
            exit(1);
        }
    }

    for (const auto &[u, v] : g.edges())
    {
        if (color.at(u) == color.at(v))
        {
            std::cout << "Same color: " << u << " " << v << std::endl;
            exit(1);
        }
    }
    std::cout << "Coloring verified!" << std::endl;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "./main filename.instance.json" << std::endl;
        return 1;
    }

    Graph<int> g = readGraph(argv[1]);
    std::cout << "Graph vertices: " << g.countVertices() << std::endl;
    std::cout << "Graph edges: " << g.countEdges() << std::endl;
    std::unordered_map<int, int> color = greedyColor(g);
    testColor(g, color);

    return 0;
}
