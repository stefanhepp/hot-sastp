all: Report_ex1.pdf Report_ex2.pdf

CSVFILES=$(shell find . -iname "*.csv")

GRAPHS=$(shell find graphs -iname "*.gnu" | sed "s/.gnu$$/.pdf/")


.SUFFIXES: .gnu .pdf
.gnu.pdf: graphs/results.gp $< $(CSVFILES)
	cd $(shell dirname $<) && gnuplot $(shell basename $<)

.PHONY: graphs
graphs: $(GRAPHS)

Report_ex1.pdf: Report_ex1.tex $(CSVFILES) $(GRAPHS) 
	rubber -fd Report_ex1.tex

Report_ex2.pdf: Report_ex2.tex $(CSVFILES) $(GRAPHS) 
	rubber -fd Report_ex2.tex

clean:
	rm -rf *.log *.pdf *.aux graphs/*.pdf
