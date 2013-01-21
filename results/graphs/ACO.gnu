set output "ACO.pdf"
set terminal pdfcairo font ",6"
load "results.gp"

plot 'ants.h1.csv' using 3:2 t " Insert ", \
     'ants.h1.p02.csv' using 3:2 t " Insert , rho=0.2"

