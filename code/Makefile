#Simple makefile for building VENT

IDIR=inc
SDIR=src
ODIR?=$(SDIR)/obj
DDIR=$(SDIR)/obj_d

CC=gcc
CFLAGS?=-I$(IDIR)
DFLAGS= $(CFLAGS) -g -O0 -DDEBUG

_DEPS = display.h error.h lexer.h token.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = display.o error.o lexer.o 
OBJ ?= $(patsubst %,$(ODIR)/%,$(_OBJ))

MAIN=$(ODIR)/main.o

#this is the VENT Transpiler executable
tvt: $(OBJ) $(MAIN)
	$(CC) -o $@ $> $^ $(CLFAGS)

tvt_d: $(OBJ) $(MAIN)
	$(CC) -o $@ $> $^ $(CFLAGS)

$(ODIR):
	mkdir -p $@
 
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(MAIN) : main.c
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: help debug test runtest clean cleand cleant cleanall print

help:
	@echo "  build tvt: 'make tvt' or just 'make'"
	@echo "  build tvt with debug symbols: 'make debug'"
	@echo "  build unit test application: 'make test'"
	@echo "  build and run unit tests: 'make runtest'"
	@echo "  "
	@echo "  clean output products: 'make clean'"
	@echo "  clean debug output products: 'make cleand'"
	@echo "  clean test output products: 'make cleant'"
	@echo "  clean everything: 'make cleanall'"

debug: 
	$(MAKE) tvt_d CFLAGS="$(DFLAGS)" ODIR=$(DDIR)

test:
	$(MAKE) -C ./test

runtest: 
	@$(MAKE) -C ./test --silent
	@./test/allTests
	@$(MAKE) -C ./test clean --silent

clean:
	rm -fr $(ODIR)
	rm -f tvt

cleand:
	rm -fr $(DDIR)
	rm -f tvt_d

cleant:
	$(MAKE) -C ./test clean 
	
cleanall: clean cleand cleant

print:
	@echo $(CFLAGS)
	@echo $(DEPS)
	@echo $(OBJ)
