CFLAGS += -Wall -std=c99

.PHONY: all install clean test

HXS := $(wildcard *.hx)
SOURCES := $(wildcard *.xml)
HTMLs := $(SOURCES:.xml=.html)

all: ${HTMLs} hx.c

hx.c: ${HXS} ${SOURCES}
	@echo "hx index.hx"
	@hx index.hx

hx: hx.c

install: hx
	cp hx ~/bin

test: hx
	./hx </dev/null

%.html: %.xml 
	@echo "xmllint $^"
	@xmllint --noout $^ || true
	@echo "sed >$@"
	@sed	\
		-e '1s/^/<!doctype html>\n/' \
		-e 's/1{}/<span class="in1"><\/span>/g' \
		-e 's/2{}/<span class="in2"><\/span>/g' \
		-e 's/3{}/<span class="in3"><\/span>/g' \
		-e 's/4{}/<span class="in4"><\/span>/g' \
		-e 's/5{}/<span class="in5"><\/span>/g' \
		-e 's/b{}/<br\/>/g' \
		-e 's/f{\([^}]*\)}/<span class="fn">\1<\/span>/g' \
		-e 's/v{\([^}]*\)}/<span class="var">\1<\/span>/g' \
		-e 's/s{\([^}]*\)}/<span class="str">\1<\/span>/g' \
		-e 's/n{\([^}]*\)}/<span class="num">\1<\/span>/g' \
		-e 's/t{\([^}]*\)}/<span class="type">\1<\/span>/g' \
		-e 's/k{\([^}]*\)}/<span class="keyword">\1<\/span>/g' \
		-e 's/a{\([^}]*\)}/<span class="add">@add(<span class="macro-name">\1<\/span>)<\/span>/g' \
		-e 's/x{\([^}]*\)}/<span class="end">@end(<span class="macro-name">\1<\/span>)<\/span>/g' \
		-e 's/e{\([^}]*\)}/<span class="expand">@expand(<span class="macro-name">\1<\/span>)<\/span>/g' \
		-e 's/i{\([^}]*\)\.xml}/<span class="expand">@include(<a href="\1.html" class="macro-name">\1.xml<\/a>)<\/span>/g' \
		$^ >$@

clean:
	rm -f hx.c ${HTMLs}
