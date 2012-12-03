set output "local.pdf"
set terminal pdfcairo font ",6"
load "results.gp"

plot 'ls.oneopt.best.csv' using 3:2 t "SpotOneOpt, Best Step", \
     'ls.oneopt.next.csv' using 3:2 t "SpotOneOpt, Next Step", \
     'ls.oneopt.rand.csv' using 3:2 t "SpotOneOpt, Random Step", \
     'ls.edgeopt.best.csv' using 3:2 t "EdgeTwoOpt, Best Step", \
     'ls.edgeopt.next.csv' using 3:2 t "EdgeTwoOpt, Next Step", \
     'ls.edgeopt.rand.csv' using 3:2 t "EdgeTwoOpt, Random Step", \
     'ls.methodopt.best.csv' using 3:2 t "MethodTwoOpt, Best Step", \
     'ls.methodopt.next.csv' using 3:2 t "MethodTwoOpt, Next Step", \
     'ls.methodopt.rand.csv' using 3:2 t "MethodTwoOpt, Random Step"


