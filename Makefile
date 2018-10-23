CFLAGS += -Wall -std=c99

.PHONY: all install clean test run_hx

Xs := $(wildcard *.x)
HTMLs := ${Xs:.x=.html}

all: ${HTMLs} hx.c

run_hx: ${HXS}
	@echo "hx index.x"
	@hx index.x

hx.c: run_hx

hx: hx.c

install: hx
	cp hx ~/bin

test: hx
	./hx </dev/null

%.html: %.x
	hx $^

clean:
	rm -f hx.c ${HTMLs}
