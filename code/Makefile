#Simple makefile for building VENT

IDIR=inc
SDIR=src
ODIR?=$(SDIR)/obj
DDIR=$(SDIR)/obj_d

CC=gcc
CFLAGS?=-I$(IDIR)
DFLAGS= $(CFLAGS) -g -O0 -DDEBUG

_DEPS = display.h token.h dba.h dht.h ast.h parser.h emitter.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = display.o lexer.o dba.o dht.o ast.o emitter.o
OBJ ?= $(patsubst %,$(ODIR)/%,$(_OBJ))

_POBJ = parser_mod.o
POBJ ?= $(patsubst %,$(ODIR)/%,$(_POBJ))

MAIN=$(ODIR)/main.o

#this is the VENT Transpiler executable
tvt: $(OBJ) $(POBJ) $(MAIN)
	$(CC) -o $@ $^ $(CLFAGS)

tvt_d: $(OBJ) $(POBJ) $(MAIN)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR):
	mkdir -p $@
 
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(POBJ): 
	$(MAKE) -C ./src/parser

$(MAIN) : main.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: help debug test again runtest checkleaks checksyntax clean cleand cleant cleanv cleanall linecount todo print updateVimSyntax

help:
	@echo "  build tvt: 'make tvt' or just 'make'"
	@echo "  build tvt with debug symbols: 'make debug'"
	@echo "  build unit test application: 'make test'"
	@echo "  build and run unit tests: 'make runtest'"
	@echo "  "
	@echo "  clean output products: 'make clean'"
	@echo "  clean everything: 'make cleanall'"
	@echo "  clean debug output products: 'make cleand'"
	@echo "  clean test output products: 'make cleant'"
	@echo "  clean parser intermediate products: 'make cleanp'"
	@echo "  clean vhdl output products: 'make cleanv'"
	@echo "  "
	@echo "  check for memory leaks: 'make checkleaks'"
	@echo "  check syntax of all VHDL files in code dir: 'make checksyntax'"
	@echo "  display number of lines in source code: 'make linecount'"
	@echo "  display active items in todo list: 'make todo'"
	@echo "  update vent vim syntax file (must be run as sudo): 'updateVimSyntax'"
	@echo "  print flags 'make print'"

debug: 
	$(MAKE) debug -C ./src/parser
	$(MAKE) tvt_d CFLAGS="$(DFLAGS)" ODIR=$(DDIR)

test:
	$(MAKE) -C ./test

again:
	@$(MAKE) cleanall --silent
	@$(MAKE)	

runtest: 
	@$(MAKE) cleanall --silent
	@$(MAKE) -C ./test --silent
	@clear && ./test/UnitTests
	@$(MAKE) -C ./test clean --silent

checkleaks:
	@$(MAKE) cleanall --silent
	@$(MAKE) -C ./test --silent
	clear && valgrind --leak-check=full ./test/UnitTests
	@$(MAKE) -C ./test clean --silent

checksyntax:
	@echo checking vhdl syntax
	@/tools/Xilinx/Vivado/2022.1/bin/vivado -mode batch -nolog -nojournal \-quiet -source ./test/check_vent.tcl -notrace -tclargs *.vhdl

clean:
	rm -fr $(ODIR)
	rm -f tvt

cleand:
	rm -fr $(DDIR)
	rm -f tvt_d

cleanp:
	$(MAKE) -C ./src/parser clean 

cleant:
	$(MAKE) -C ./test clean 

cleanv:
	rm -f *.vhdl	

cleanall: clean cleand cleanp cleant cleanv

linecount:
	wc -l inc/*.* src/*.* src/parser/*.* main.c

linecountAll:
	wc -l inc/*.* src/*.* src/parser/*.* test/*_test*.c main.c

updateVimSyntax:
	cp ../docs/vent.vim /usr/share/vim/vim82/syntax/

todo:
	@clear && grep -v '###' ../docs/todo.txt | grep -v -e ^[[:space:]]*\$

print:
	@echo $(CFLAGS)
	@echo $(DEPS)
	@echo $(OBJ)
