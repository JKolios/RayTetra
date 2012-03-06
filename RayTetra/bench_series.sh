#!/bin/bash

#Establish a clean testing env
make clean cleanbak cleanoutput
make all

#50M Series
#Main Benchmark
./fullbench.sh 5000000 10 10

#Caching Benchmarks
./fullbench.sh 500000 100 10
./fullbench.sh 50000 1000 10

#25M Series
#Main Benchmark
./fullbench.sh 2500000 10 10

#Caching Benchmarks
./fullbench.sh 250000 100 10
./fullbench.sh 25000 1000 10
