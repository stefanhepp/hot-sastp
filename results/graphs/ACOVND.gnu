set output "ACOVND.pdf"
set terminal pdfcairo font ",6"
load "results.gp"

plot 'ants.vnd.h0.csv' using 3:2 t " Next", \
     'ants.vnd.h1.csv' using 3:2 t " Insert ", \
     'ants.vnd.h0.p02.csv' using 3:2 t " Next , rho=0.2", \
     'ants.vnd.h1.p02.csv' using 3:2 t " Insert , rho=0.2"

