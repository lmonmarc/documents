# for d in */ ; do\
# 	rm -f $$d/*.pdf;\
# 	rm -f $$d/*.out;\
# 	rm -f $$d/*.aux;\
# 	rm -f $$d/*.log;\
# done;\


SRCS:=$(wildcard */main.tex)
RES:=$(patsubst %.tex,%.pdf,$(SRCS))

all: $(RES)

%/main.pdf: %/main.tex %/text.tex
	@echo `dirname  $<`
	@cd `dirname  $<` ;\
	pdflatex main.tex ;\

clean:
	rm -f **/*.pdf **/*.out **/*.aux **/*.log

