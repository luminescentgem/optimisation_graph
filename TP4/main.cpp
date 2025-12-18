/*
g++ -Wall -c -I/opt/ibm/ILOG/CPLEX_Studio221/cplex/include -I /opt/ibm/ILOG/CPLEX_Studio221/concert/include main.cpp

g++ -o ind-cplex -L/opt/ibm/ILOG/CPLEX_Studio221/cplex/lib/x86-64_linux/static_pic -L/opt/ibm/ILOG/CPLEX_Studio221/concert/lib/x86-64_linux/static_pic main.o -lilocplex -lconcert -lcplex -lpthread -ldl
*/
#include "Solver.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using Vertex = long long int;
double maxtime = 20;

int main(int argc, char **argv) {
  if(argc != 2) {
    cout << "./ind-cplex <inputfile>" << endl;
    exit(1);
  }

  Graph<Vertex> g(argv[1]); // Read input graph
  cout << "Read input graph with " << g.countVertices() << " vertices and "
                                   << g.countEdges() << " edges" << endl;

  std::unordered_set<Vertex> solution;

  while(elapsed() < maxtime) {
    int iterations = 0;
    Solver<Vertex> solver(g);
    
    solver.solve_greedy();

    std::cout << "Independant set size: "
              << solver.solution().size()
              << std::flush;
    
    double improved = elapsed();

    while(elapsed() < maxtime
          && elapsed() - improved < maxtime / 8
          && !solver.solved()
    ) {
      if(solver.improve()) {
        std::cout << " -> " << solver.solution().size() << std::flush;
        improved = elapsed();
      }
      iterations++;
    }

    if(solution.empty() || solver.solution().size() > solution.size())
      solution = solver.solution();
    
    std::cout << std::endl
              << "After " << iterations << " iterations,"
              << " we found a independant set of size "
              << solver.solution().size()
              << " and the best size found is "
              << solution.size()
              << std::endl;
    
    if (solver.solved()) {
      std::cout << std::endl
                << "Solution is already optimal, stopping early."
                << std::endl;
      break;
    }
  }

  string outfn = argv[1]; // Create filename for output
  outfn.replace(outfn.end()-5, outfn.end(), "ind");
  save(outfn, solution);
  
  return 0;
}
