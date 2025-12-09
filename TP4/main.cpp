/*
g++ -Wall -c -I/opt/ibm/ILOG/CPLEX_Studio221/cplex/include -I /opt/ibm/ILOG/CPLEX_Studio221/concert/include main.cpp

g++ -o ind-cplex -L/opt/ibm/ILOG/CPLEX_Studio221/cplex/lib/x86-64_linux/static_pic -L/opt/ibm/ILOG/CPLEX_Studio221/concert/lib/x86-64_linux/static_pic main.o -lilocplex -lconcert -lcplex -lpthread -ldl
*/
#include "Solver.hpp"
#include <iostream>
#include <fstream>

using namespace std;
using Vertex = long long int;

double maxtime = 60;

int main(int argc, char **argv) {
  if(argc != 2) {
    cout << "./ind-cplex inputfile" << endl;
    exit(1);
  }

  Graph<Vertex> g(argv[1]); // Read input graph

  Solver solver(g, maxtime);

  solver.solve();

  string outfn = argv[1]; // Create filename for output
  outfn.replace(outfn.end()-5, outfn.end(), "ind");

  solver.save(outfn);

  return 0;
}
