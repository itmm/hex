CXXFLAGS += -Wall -std=c++14 -g

.PHONY: all install clean test

Xs := $(wildcard *.x)
HTMLs := ${Xs:.x=.html}

all: hx.cc

hx.cc: ${Xs}
	hx

hx: hx.cc

install: hx
	cp hx ~/bin

test: hx
	./hx

clean:
	rm -f hx.cc ${HTMLs}
