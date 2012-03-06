#!/bin/bash
gnuplot << EOF

#Output settings
set terminal epslatex 
set output '$1_total.eps'

# Scale and aspect ratio
set size 1.3,1

# Line width of the axes
set border linewidth 1.5

# Line styles
set style line 1 linecolor rgb '#006400' linetype 1 linewidth 2 pointtype 1
set style line 2 linecolor rgb '#FF0000' linetype 1 linewidth 2 pointtype 2
set style line 3 linecolor rgb '#104E8B' linetype 1 linewidth 2 pointtype 3
set style line 4 linecolor rgb '#EE7600' linetype 1 linewidth 2 pointtype 4
set style line 5 linecolor rgb '#006400' linetype 1 linewidth 2 pointtype 5
set style line 6 linecolor rgb '#FF0000' linetype 1 linewidth 2 pointtype 6
set style line 7 linecolor rgb '#104E8B' linetype 1 linewidth 2 pointtype 7
set style line 8 linecolor rgb '#EE7600' linetype 1 linewidth 2 pointtype 8

# Labels and legend
set key outside right bottom
set title "Συνολικός Χρόνος Εκτέλεσης" 
set xlabel 'Ποσοστό τεμνόμενων ζευγών \%'
set ylabel "Χρόνος (ms)"

# Data file settings
set datafile commentschars "#"
set datafile separator ","

# Plot
# data columns = 6,9,10,11,13,16,19,22
plot "$1.csv" using 1:6 title "Plücker 0 CPU" with linespoints linestyle 1,\
"$1.csv" using 1:9 title "STP 0 CPU" with linespoints linestyle 2,\
"$1.csv" using 1:10 title "STP 1 CPU" with linespoints linestyle 3,\
"$1.csv" using 1:11 title "STP 2 CPU" with linespoints linestyle 4,\
"$1.csv" using 1:(\$12+\$13+\$14) title "Plücker 0 GPU" with linespoints linestyle 5,\
"$1.csv" using 1:(\$15+\$16+\$17) title "STP 0 GPU" with linespoints linestyle 6,\
"$1.csv" using 1:(\$18+\$19+\$20) title "STP 1 GPU" with linespoints linestyle 7,\
"$1.csv" using 1:(\$21+\$22+\$23) title "STP 2 GPU" with linespoints linestyle 8

EOF
