.PHONY: all bg

CFLAGS += -std=c11 -Wall
Xs := $(wildcard *.x)

all: blocks_bg ../bg.svg

blocks_bg.c: $(Xs)
	hx --css=slides.css

blocks_bg: blocks_bg.c
	$(CC) $(CFLAGS) $^ -lm -o $@

../bg.svg: blocks_bg
	./blocks_bg >$@

bg: blocks_bg
	./blocks_bg >bg.svg
