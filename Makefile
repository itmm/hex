CXXFLAGS += -Wall -std=c++14 -g

.PHONY: all install clean test

Xs := $(wildcard *.x)
HTMLs := ${Xs:.x=.html}

all: hx.cpp

hx.cpp: ${Xs}
	hx

hx: hx.cpp

install: hx
	cp hx ~/bin

test: hx
	./hx

clean:
	rm -f hx.cpp ${HTMLs}
