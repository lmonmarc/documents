all: main.pdf

main.pdf: *.tex art.bib online.bib ths.bib 
	pdflatex -shell-escape main.tex
	biber main
	#bibtex main
	pdflatex -shell-escape main.tex
	pdflatex -shell-escape main.tex

clean: 
	rm -f *.aux *.log *~ *.bbl *.blg *.dvi *.toc *.pdf *.out *.bcf *.run.xml
