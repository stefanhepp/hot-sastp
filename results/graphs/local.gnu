set output "local.pdf"
set terminal pdfcairo font ",6"
load "results.gp"

plot 'greedy.nn.csv' using 3:2 t "NN", 'greedy.ih.csv' using 3:2 t "IH"

