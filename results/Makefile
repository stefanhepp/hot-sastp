all: Report.pdf

CSVFILES=$(shell find . -iname "*.csv")

Report.pdf: Report.tex $(CSVFILES)
	rubber -d Report.tex
