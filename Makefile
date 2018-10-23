CFLAGS += -Wall -std=c99 -g

.PHONY: all install clean test

Xs := $(wildcard *.x)
HTMLs := ${Xs:.x=.html}

all: hx.c

hx.c: ${Xs}
	hx

hx: hx.c

install: hx
	cp hx ~/bin

test: hx
	./hx </dev/null

clean:
	rm -f hx.c ${HTMLs}
