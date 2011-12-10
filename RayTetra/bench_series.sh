#!/bin/bash

#Establish a clean testing env
make clean cleanbak cleanoutput
make all

#Main Benchmark
./full_bench 1512000 10 10

#Caching Benchmarks
./full_bench 1000000 1 10
./full_bench 100000 10 10
./full_bench 10000 100 10
./full_bench 1000 1000 10
./full_bench 100 10000 10
./full_bench 10 100000 10
./full_bench 1 1000000 10
