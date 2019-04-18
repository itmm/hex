.PHONY: all clean

Xs := $(wildcard *.x)
SRCs := ../hx.cpp
DOCs := $(Xs:.x=.html)

all: $(SRCs)

$(SRCs): $(Xs)
	@echo "  HX"
	@hx

clean:
	@echo "  RM generated files"
	@rm -f $(SRCs) $(DOCs)
