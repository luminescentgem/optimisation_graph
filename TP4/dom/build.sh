#!/bin/bash

# Compile without CMAKE

g++ -Wall -c -I/opt/ibm/ILOG/CPLEX_Studio221/cplex/include -I /opt/ibm/ILOG/CPLEX_Studio221/concert/include main.cpp

g++ -o main -L/opt/ibm/ILOG/CPLEX_Studio221/cplex/lib/x86-64_linux/static_pic -L/opt/ibm/ILOG/CPLEX_Studio221/concert/lib/x86-64_linux/static_pic main.o -lilocplex -lconcert -lcplex -lpthread -ldl


# Compile with CMAKE
#
# cmake -DCPLEX_ROOT_DIR=/opt/ibm/ILOG/CPLEX_Studio221/ -DCMAKE_MODULE_PATH=.  CMakeLists.txt
#
# make


# Run

for a in ../instances/*.edges
do
  time ./main $a
  python3 testdom.py $a ${a%.edges}.dom
done

