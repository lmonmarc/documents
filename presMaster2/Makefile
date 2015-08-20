# package Pygments (for python) must be installed
# -shell-escape is used for the minted package which uses Pygments

SRCS1:=$(wildcard */main-rep.tex)
RES1:=$(patsubst %.tex,%.pdf,$(SRCS1))

SRCS2:=$(wildcard */main-pres.tex)
RES2:=$(patsubst %.tex,%.pdf,$(SRCS2))

all: $(RES1) $(RES2)

%/main-rep.pdf: %/*.tex
	@echo "Compiling $@ with pdflatex -shell-escape ..."
	@echo "pdflatex used of unicode characters"
	@cd `dirname  $<` ;\
	pdflatex -shell-escape main-rep.tex > /dev/null ;\
	pdflatex -shell-escape main-rep.tex > /dev/null ;\
	pdflatex -shell-escape main-rep.tex > /dev/null ;\


%/main-pres.pdf: %/*.tex
	@echo "Compiling $@ with xelatex -shell-escape ..."
	@echo "xelatex used for special fonts"
	@cd `dirname  $<` ;\
	xelatex -shell-escape main-pres.tex > /dev/null ;\
	xelatex -shell-escape main-pres.tex > /dev/null ;\
	xelatex -shell-escape main-pres.tex > /dev/null ;\

clean:
	rm -f **/*.pdf **/*.out **/*.aux **/*.log \
	**/*.nav **/*.pyg **/*.snm **/*.toc **/*.vrb
