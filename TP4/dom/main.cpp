/*
g++ -Wall -c -I/opt/ibm/ILOG/CPLEX_Studio2211/cplex/include -I /opt/ibm/ILOG/CPLEX_Studio2211/concert/include main.cpp

g++ -o dom-cplex -L/opt/ibm/ILOG/CPLEX_Studio2211/cplex/lib/x86-64_linux/static_pic -L/opt/ibm/ILOG/CPLEX_Studio2211/concert/lib/x86-64_linux/static_pic main.o -lilocplex -lconcert -lcplex -lpthread -ldl
*/
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include "graph.hpp"
#include <ilcplex/ilocplex.h>

using namespace std;
using Vertex = long long int;

double maxtime = 120;

unordered_set<Vertex> exact(const Graph<Vertex> &g) {
  IloEnv env;

  cout << "Creating variables" << endl;
  unordered_map<Vertex,IloNumVar> variables;
  for(Vertex v : g.vertices()) {
    variables[v] = IloNumVar(env, 0, 1, ILOINT);
  }

  IloModel model(env);

  cout << "Creating constraints" << endl;
  // The sum of the neighbors of each vertex must be at least 1
  for(Vertex v : g.vertices()) {
    IloExpr expr(env);
    expr += variables[v];
    for(Vertex u : g.neighbors(v)) {
      expr += variables[u];
    }
    model.add(expr >= 1);
  }

  cout << "Setting objective" << endl;
  // Minimize the number of vertices
  IloExpr expr(env);
  for(Vertex v : g.vertices())
    expr += variables[v];
  model.add(IloMinimize(env,expr));

  cout << "Solving" << endl;
  IloCplex cplex(model);
  cplex.setOut(env.getNullStream()); // Disable console output
  cplex.setParam(IloCplex::Param::TimeLimit, maxtime); // 60 second time limit
  bool solved = cplex.solve();

  cout << "Saving solution" << endl;
  unordered_set<Vertex> dom;
  if(solved) {
    cout << "Solution is " << cplex.getCplexStatus() << endl;
  // Pick Vertices of value 1 and add to dom
    for(Vertex v : g.vertices()) {
      if(cplex.getValue(variables[v]) > .5)
        dom.insert(v);
    }
  }
  env.end(); // Free memory, as the destructor does not free it!

  return dom;
}

// Save the solution to a file
void save(const std::string &outfn, std::unordered_set<Vertex> solution) {
  std::ofstream outfile(outfn);
  for(Vertex v : solution) {
    outfile << v << std::endl;
  }
  std::cout << "Saved a dominating set of size " << solution.size() << std::endl;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    cout << "./dom-cplex inputfile" << endl;
    exit(1);
  }

  Graph<Vertex> g(argv[1]); // Read input graph
  string outfn = argv[1]; // Create filename for output
  outfn.replace(outfn.end()-5, outfn.end(), "dom");

  unordered_set<Vertex> dom = exact(g);
  cout << "Saved dominating unordered_set of size " << dom.size() << " to " << outfn << endl;
  save(outfn, dom);

  return 0;
}
