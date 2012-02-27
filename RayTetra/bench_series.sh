#!/bin/bash

#Establish a clean testing env
make clean cleanbak cleanoutput
make all

#Main Benchmark
./fullbench.sh 5000000 10 10

#Caching Benchmarks
./fullbench.sh 500000 100 10
./fullbench.sh 50000 1000 10
