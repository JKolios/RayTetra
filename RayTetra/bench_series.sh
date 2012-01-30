#!/bin/bash

#Establish a clean testing env
make clean cleanbak cleanoutput
make all

#Main Benchmark
./fullbench.sh 1512000 10 10

#Caching Benchmarks
./fullbench.sh 150000 100 10
./fullbench.sh 15000 1000 10
