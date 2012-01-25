#!/bin/bash
gnuplot << EOF

# Output settings
set terminal pngcairo size 1000,600 enhanced font 'Verdana,10'
set output '$1_bar.png'

# Formatting
set border 3 front linetype -1 linewidth 1.000
set boxwidth 0.75 absolute
set style fill   solid 1.00 border -1
set grid nopolar
set grid noxtics nomxtics ytics nomytics noztics nomztics \
 nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
set grid layerdefault   linetype 0 linewidth 1.000,  linetype 0 linewidth 1.000
set key outside right top vertical Left reverse enhanced autotitles columnhead box linetype -1 linewidth 1.000
set style histogram columnstacked title  offset character 0, 0, 0
set datafile missing '-'
set style data histograms
set xtics border in scale 1,0.5 nomirror norotate  offset character 0, 0, 0 
set xtics   ("Plücker 0 GPU " 0.500000, "STP 0 GPU" 1.50000, "STP 1 GPU" 2.50000, "STP 2 GPU" 3.50000)
set ytics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autofreq 
set ztics border in scale 0,0 nomirror norotate  offset character 0, 0, 0 autofreq 
set cbtics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autofreq 
set title "Ανάλυση Χρόνου Εκτέλεσης σε GPU " 
set xlabel "Αλγόριθμος" 
set ylabel "Χρόνος (ms)" 
set yrange [ 0.00000 : * ] noreverse nowriteback  # (currently [:100.000] )

# Data file settings
set datafile commentschars "#"
set datafile separator ","

i = 24
plot "$1" using 12, '' using 13,'' using 14

EOF
