#!/bin/bash
gnuplot << EOF

#Output settings
set terminal pngcairo size 1000,600 enhanced font 'Verdana,10'
set output '$1.png'

# Line width of the axes
set border linewidth 1.5

# Line styles
set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 2
set style line 2 linecolor rgb '#dd181f' linetype 1 linewidth 2
set style line 3 linecolor rgb '#0060ad' linetype 2 linewidth 2
set style line 4 linecolor rgb '#dd181f' linetype 2 linewidth 2
set style line 5 linecolor rgb '#0060ad' linetype 3 linewidth 2
set style line 6 linecolor rgb '#dd181f' linetype 3 linewidth 2
set style line 7 linecolor rgb '#0060ad' linetype 4 linewidth 2
set style line 8 linecolor rgb '#dd181f' linetype 4 linewidth 2

# Labels and legend
set key outside right bottom
set title "Συνολικός Χρόνος Επεξεργασίας" 
set xlabel "Ποσοτό τεμνόμενων ζευγών %"
set ylabel "Συνολικός Χρόνος (ms)"

# Data file settings
set datafile commentschars "#"
set datafile separator ","

# Plot
# data columns = 6,9,10,11,13,16,19,22
plot "$1" using 1:6 title "Plücker 0 CPU" with linespoints linestyle 1,\
"$1" using 1:9 title "STP 0 CPU" with linespoints linestyle 2,\
"$1" using 1:10 title "STP 1 CPU" with linespoints linestyle 3,\
"$1" using 1:11 title "STP 2 CPU" with linespoints linestyle 4,\
"$1" using 1:13 title "Plücker 0 GPU" with linespoints linestyle 5,\
"$1" using 1:16 title "STP 0 GPU" with linespoints linestyle 6,\
"$1" using 1:19 title "STP 1 GPU" with linespoints linestyle 7,\
"$1" using 1:22 title "STP 2 GPU" with linespoints linestyle 8

EOF
