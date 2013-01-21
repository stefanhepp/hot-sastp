set output "ACONEXT.pdf"
set terminal pdfcairo font ",6"
load "results.gp"

plot 'ants.h0.csv' using 3:2 t " Next", \
     'ants.h0.p02.csv' using 3:2 t " Next , rho=0.2"
    
