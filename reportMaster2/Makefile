all: main.pdf

main.pdf: *.tex art.bib online.bib ths.bib 
	pdflatex main.tex
	biber main
	#bibtex main
	pdflatex main.tex
	pdflatex main.tex

clean: 
	rm -f *.aux *.log *~ *.bbl *.blg *.dvi *.toc *.pdf *.out *.bcf *.run.xml
