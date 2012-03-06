#!/bin/bash


for file in *.csv
do
ext=csv
./plot_time_graph.sh ${file%.$ext}
./plot_total_time_graph.sh ${file%.$ext}
done
