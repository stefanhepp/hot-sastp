set output "greedy.pdf"
set terminal pdfcairo font ",6"
#set size ratio 0.65 # 1.5,1.5
load "results.gp"

plot 'greedy.nn.csv' using 3:2 t "NN", 'greedy.ih.csv' using 3:2 t "IH"

