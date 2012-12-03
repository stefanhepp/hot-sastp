all: Report.pdf

CSVFILES=$(shell find . -iname "*.csv")

GRAPHS=$(shell find graphs -iname "*.gnu" | sed "s/.gnu$$/.pdf/")


.SUFFIXES: .gnu .pdf
.gnu.pdf: graphs/results.gp
	cd $(shell dirname $<) && gnuplot $(shell basename $<)

.PHONY: graphs
graphs: $(GRAPHS)

Report.pdf: Report.tex $(CSVFILES) $(GRAPHS) 
	rubber -fd Report.tex
