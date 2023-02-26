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

.PHONY: help debug test runtest clean cleand cleanall print

help:
	@echo "call 'make tvt' or just 'make'"

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

cleanall: clean cleand

print:
	@echo $(CFLAGS)
	@echo $(DEPS)
	@echo $(OBJ)
