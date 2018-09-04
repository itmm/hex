CFLAGS += -Wall -std=c99

.PHONY: all clean

all: hex
	./hex

hex: hex.c

clean:
	rm -f hex
