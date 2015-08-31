gnuplot <<EOF

set terminal pdf truecolor size 10,5

set boxwidth 0.7 absolute
set style data histograms 
set style fill solid 1.00 border lt -1
set style fill transparent solid 0.9 noborder
set style histogram rowstacked
set datafile missing '-'

set xtics border in scale 0,0 nomirror rotate by -60  autojustify
set xtics ()
set xtics norangelimit
set xtics font "Verdana,14" 

set grid
#set size 1,1

set yrange [0:*] noreverse nowriteback
set xlabel "Programme exécuté"

set key on left
set output "graph1.pdf" 
set ylabel "Temps d'exécution (ms)"
set title "Temps d'exécution normalisé de la version OpenMP avec tuilage en fonction de la taille des macro-tuiles (M) et des tuiles (T)"
plot "graph1et2.dat" u ((\$4+\$14)/(2.0**\$22)):xtic(1)  lc rgb '#DE77AE' ti col(4), '' u ((\$3+\$13)/(2.0**\$22))  lc rgb '#7570B3' ti col(3), '' u ((\$5+\$15)/(2.0**\$22))  lc rgb '#1B9E77' ti col(5), '' u ((\$2+\$12)/(2.0**\$22))  lc rgb '#E6AB02' ti col(2)

set key on right
set output "graph2.pdf" 
set ylabel "Nombre de cache-misses"
set title "Nombre de cache-misses de la version OpenMP avec tuilage en fonction de la taille des macro-tuiles (M) et des tuiles (T)"
plot "graph1et2.dat" u ((\$9+\$19)/(2.0**\$22))  lc rgb '#33A02C' ti col(9), '' u ((\$10+\$20)/(2.0**\$22))  lc rgb '#1F78B4' ti col(10), '' u ((\$11+\$21)/(2.0**\$22))  lc rgb '#E31A1C' ti col(11), '' u ((\$6+\$16)/(2.0**\$22)):xtic(1)  lc rgb '#B2DF8A' ti col(6), '' u ((\$7+\$17)/(2.0**\$22))  lc rgb '#A6CEE3' ti col(7), '' u ((\$8+\$18)/(2.0**\$22))  lc rgb '#FB9A99' ti col(8)


EOF
