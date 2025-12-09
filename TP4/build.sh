#!/bin/bash

# compile
g++ -std=c++20 -Wfatal-errors -c \
  -I/opt/ibm/ILOG/CPLEX_Studio221/cplex/include \
  -I/opt/ibm/ILOG/CPLEX_Studio221/concert/include \
  main.cpp

g++ -std=c++20 -o main \
  -L/opt/ibm/ILOG/CPLEX_Studio221/cplex/lib/x86-64_linux/static_pic \
  -L/opt/ibm/ILOG/CPLEX_Studio221/concert/lib/x86-64_linux/static_pic \
  main.o -lilocplex -lconcert -lcplex -lpthread -ldl

for a in instances/*qatar*.edges
do
  time ./main $a
  # python3 testind.py $a ${a%.edges}.ind
done

