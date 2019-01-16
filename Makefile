CXXFLAGS += -Wall -std=c++14 -g

.PHONY: all install clean test

Xs := $(wildcard *.x)
SRCs := $(shell hx-files.sh $(Xs))
EXEs := hx
DOCs := $(Xs:.x=.html)

all: $(SRCs) $(DOCs)

$(SRCs): $(Xs)
	@echo "  HX"
	@hx

$(EXEs): $(SRCs)
	@echo "  CXX $@"
	@$(CXX) $(CXXFLAGS) -o $@ $^

install: $(EXEs)
	@echo "  INSTALL"
	@cp $(EXEs) hx-files.sh ~/bin

test: $(EXEs)
	@echo "  ./HX"
	@./hx

clean:
	@echo "  RM generated files"
	@rm -f $(SRCs) $(EXEs) $(DOCs)
