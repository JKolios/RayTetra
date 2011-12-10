#!/bin/bash

#Establish a clean testing env
make clean cleanbak cleanoutput
make all

#Main Benchmark
./fullbench.sh 1512000 10 10

#Caching Benchmarks
./fullbench.sh 1000000 1 10
./fullbench.sh 100000 10 10
./fullbench.sh 10000 100 10
./fullbench.sh 1000 1000 10
./fullbench.sh 100 10000 10
./fullbench.sh 10 100000 10
./fullbench.sh 1 1000000 10
