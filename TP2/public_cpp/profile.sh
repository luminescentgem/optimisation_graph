#!/bin/bash
inputfile=../input/us-night-20000.instance.json
outputfile=../input/us-night-20000.solution.svg

g++ main.cpp -std=c++20 -o main -fno-omit-frame-pointer -fno-inline-functions -fno-inline-functions-called-once -fno-default-inline -g -pg
rm gmon.out
./main $inputfile $outputfile
gprof main | gprof2dot -s -n 2 | dot -Tsvg > gprof2.svg
gprof main | gprof2dot -s -n 9 | dot -Tsvg > gprof9.svg

g++ main.cpp -std=c++20 -o main -O2 -fno-omit-frame-pointer -fno-inline-functions -fno-inline-functions-called-once -fno-default-inline -g
rm callgrind.out.*
valgrind --tool=callgrind ./main $inputfile $outputfile
callgrind_annotate callgrind.out.* --inclusive=yes --auto=yes
gprof2dot -s -n 2 --format=callgrind callgrind.out.* | dot -Tsvg > callgrind2.svg
gprof2dot -s -n 9 --format=callgrind callgrind.out.* | dot -Tsvg > callgrind9.svg

