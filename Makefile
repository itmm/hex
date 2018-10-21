CFLAGS += -Wall -std=c99

.PHONY: all install clean test run_hx

HXS := $(wildcard *.hx)
HTMLs := ${HXS:.hx=.html}

all: ${HTMLs} hx.c

run_hx: ${HXS}
	@echo "hx index.hx"
	@hx index.hx

hx.c: run_hx

hx: hx.c

install: hx
	cp hx ~/bin

test: hx
	./hx </dev/null

%.html: %.hx
	hx $^

clean:
	rm -f hx.c ${HTMLs}
