all: main

main: main.tex main.bib
	pdflatex main.tex
	bibtex main
	pdflatex main.tex
	pdflatex main.tex

clean: 
	rm -f *.aux *.log *~ *.bbl *.blg *.dvi *.toc *.pdf
