#set border 3 front linetype -1 linewidth 1.000
#set boxwidth 0.9 absolute
#set style fill solid 1.00 noborder
set grid nopolar
#set grid noxtics nomxtics ytics mytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
#set grid layerdefault  linetype 1 linewidth 1.000 linecolor rgb "gray",  linetype 0 linewidth 1.000 linecolor rgb "black"
#set key top right horizontal reverse enhanced nobox Left samplen 2.5 width 0.2
set key bottom right nobox
set pointsize 0.5
set style data linespoints
set datafile separator ','

#set xtics border out scale 0,0 nomirror rotate by -65  offset character 0, 0, 0 
#set ytics border out scale 1,0.25 nomirror norotate  offset character 0, 0, 0 autofreq 
#set mytics 2
#set ztics border in scale 0,0 nomirror norotate  offset character 0, 0, 0 autofreq 
#set cbtics border in scale 0,0 mirror norotate  offset character 0, 0, 0 autofreq 
#set title "Execution times for various greedy inliner settings" 
#set xlabel "(Same plot using rowstacked rather than clustered histogram)" 
#set xlabel  offset character 0, -2, 0 font "" textcolor lt -1 norotate
set ylabel "Satisfaction" 
set xlabel "Time (s)"
#set yrange [ 0.0 : 160.0 ] noreverse nowriteback
