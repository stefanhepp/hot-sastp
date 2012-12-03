set output "grasp.pdf"
set terminal pdfcairo font ",6"
load "results.gp"

plot 'grasp.ls.oneopt.best.csv' using 3:2 t "SpotOneOpt, Best Step", \
     'grasp.ls.oneopt.next.csv' using 3:2 t "SpotOneOpt, Next Step", \
     'grasp.ls.oneopt.rand.csv' using 3:2 t "SpotOneOpt, Random Step", \
     'grasp.ls.edgeopt.best.csv' using 3:2 t "EdgeTwoOpt, Best Step", \
     'grasp.ls.edgeopt.next.csv' using 3:2 t "EdgeTwoOpt, Next Step", \
     'grasp.ls.edgeopt.rand.csv' using 3:2 t "EdgeTwoOpt, Random Step", \
     'grasp.ls.methodopt.best.csv' using 3:2 t "MethodTwoOpt, Best Step", \
     'grasp.ls.methodopt.next.csv' using 3:2 t "MethodTwoOpt, Next Step", \
     'grasp.ls.methodopt.rand.csv' using 3:2 t "MethodTwoOpt, Random Step", \
     'grasp.vnd.best.csv' using 3:2 t "VND, Best Step", \
     'grasp.vnd.next.csv' using 3:2 t "VND, Next Step"


