CXXFLAGS += -Wall -std=c++14 -g

.PHONY: all install clean test itest ctest

ifeq (,$(wildcard /usr/include/ncurses.h))
$(warning "building without ncurses support")
else
CXXFLAGS += -DWITH_NCURSES -lncurses
endif

Xs := $(wildcard *.x)
SRCs := hx.cpp
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

itest: $(EXEs)
	@echo "  ./HX -i"
	@./hx -i

ctest: $(EXEs)
	@echo "  ./HX -c"
	@./hx -c

clean:
	@echo "  RM generated files"
	@rm -f $(SRCs) $(EXEs) $(DOCs)
