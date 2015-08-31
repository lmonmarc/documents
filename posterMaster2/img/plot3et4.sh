gnuplot <<EOF

set terminal pdf truecolor size 10,5

set boxwidth 0.7 absolute
set style data histograms 
set style fill solid 1.00 border lt -1
set style fill transparent solid 0.9 noborder
set style histogram rowstacked
set datafile missing '-'

set xtics border in scale 0,0 nomirror rotate by -50  autojustify
set xtics ()
set xtics norangelimit
set xtics font "Verdana,14" 

set grid
#set size 1,1

set key on outside left
set yrange [0:*] noreverse nowriteback
set ylabel "Temps d'exécution (ms)"

set output "graph3.pdf" 
set title "Temps d'exécution normalisé des versions de base et triSYCL, avec et sans tuilage (M128-T128)"
#plot "graph3.dat" u ((\$4+\$14)/(2.0**\$22)):xtic(1)  lc rgb 'red' ti col(4), '' u ((\$3+\$13)/(2.0**\$22))  lc rgb 'blue' ti col(3), '' u ((\$5+\$15)/(2.0**\$22))  lc rgb 'green' ti col(5), '' u ((\$2+\$12)/(2.0**\$22))  lc rgb 'yellow' ti col(2)
plot "graph3.dat" u ((\$4+\$14)/(2.0**\$22)):xtic(1)  lc rgb '#DE77AE' ti col(4), '' u ((\$3+\$13)/(2.0**\$22))  lc rgb '#7570B3' ti col(3), '' u ((\$5+\$15)/(2.0**\$22))  lc rgb '#1B9E77' ti col(5), '' u ((\$2+\$12)/(2.0**\$22))  lc rgb '#E6AB02' ti col(2)

set output "graph4.pdf" 
set title "Temps d'exécution normalisé des différentes versions parallèles (M128-T128)"
#plot "graph4.dat" u ((\$4+\$14)/(2.0**\$22)):xtic(1)  lc rgb 'red' ti col(4), '' u ((\$3+\$13)/(2.0**\$22))  lc rgb 'blue' ti col(3), '' u ((\$5+\$15)/(2.0**\$22))  lc rgb 'green' ti col(5), '' u ((\$2+\$12)/(2.0**\$22))  lc rgb 'yellow' ti col(2)
plot "graph4.dat" u ((\$4+\$14)/(2.0**\$22)):xtic(1)  lc rgb '#DE77AE' ti col(4), '' u ((\$3+\$13)/(2.0**\$22))  lc rgb '#7570B3' ti col(3), '' u ((\$5+\$15)/(2.0**\$22))  lc rgb '#1B9E77' ti col(5), '' u ((\$2+\$12)/(2.0**\$22))  lc rgb '#E6AB02' ti col(2)


EOF
