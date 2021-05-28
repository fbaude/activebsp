#!/bin/bash

datafile=$1

gnuplot -persist <<-EOF

set title "Active object vector call"
set xlabel "vector size (bytes)"
set ylabel "time(s)
set format x "%.0s%c"
set format y "%.1s%c"
set yrange[0:*]
set xrange[0:*]

set term pngcairo size 1024,768
set output "${datafile}.png"

plot "$datafile" using 1:2 with lines title "time transfer",\
     "$datafile" using 1:3 with lines title "time first class",\
     "$datafile" using 1:3 with lines title "time distr vector"
EOF
