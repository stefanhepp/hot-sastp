set output "vnd.pdf"
set terminal pdfcairo font ",6"
load "results.gp"

plot 'vnd.best.csv' using 3:2 t "VND, Best Step", 'vnd.next.csv' using 3:2 t "VND, Next Step"

