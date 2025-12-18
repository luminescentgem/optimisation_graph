#pragma once

#include "Graph.hpp"
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <utility>
#include <string>
#include <ilcplex/ilocplex.h>

template <class Vertex>
using Edge = std::pair<Vertex, Vertex>;

template <class Vertex>
class SubSolver
{
public:
    Graph<Vertex>& g;

    IloEnv env;
    IloModel model;
    IloCplex cplex;

    std::unordered_set<Vertex> solution, vertices;
    std::unordered_map<Vertex, IloNumVar> variables;

public:
    SubSolver(Graph<Vertex> &g)
        : g(g),
          env(),
          model(env),
          cplex(model),
          vertices(g.vertices())
    {
        // /**/ std::cout << "Creating variables" << std::endl; /**/
        for (Vertex v : g.vertices())
            variables[v] = IloNumVar(env, 0.0, 1.0, ILOINT);

        // /**/ std::cout << "Creating constraints" << std::endl; /**/
        for (Edge<Vertex> e : g.edges()) {
            IloExpr expr(env);
            expr += variables[e.first] + variables[e.second];
            model.add(expr <= 1);
            expr.end();
        }

        // /**/ std::cout << "Setting objective" << std::endl; /**/
        IloExpr expr(env);
        for (Vertex v : g.vertices())
            expr += variables[v];
        model.add(IloMaximize(env, expr));
        expr.end();
    }

    ~SubSolver()
    {
        cplex.end();
        model.end();
        env.end();
    }

    bool solve()
    {
        // /**/ std::cout << "Solving" << std::endl; /**/
        cplex.setOut(env.getNullStream()); // Disable console output
        // cplex.setParam(IloCplex::Param::TimeLimit, maxtime);
        return cplex.solve();
    }

    bool optimal()
    {
        return cplex.getCplexStatus() == IloCplex::Optimal;
    }

    std::unordered_set<Vertex> independant()
    {
        // /**/ std::cout << "Saving solution" << std::endl; /**/
        if (!cplex.isPrimalFeasible()) {
            std::cout << "No feasible solution to save (status: " << cplex.getCplexStatus() << ")" << std::endl;
            return {};
        }

        // std::cout << "Solution is " << cplex.getCplexStatus() << std::endl;
        solution.clear();
        for (Vertex v : g.vertices()) {
            if (cplex.getValue(variables[v]) > 0.5)
                solution.insert(v);
        }

        return solution;
    }
};
