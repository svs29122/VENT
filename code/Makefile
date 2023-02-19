#Simple makefile for building VENT

IDIR=inc
SDIR=src
ODIR=$(SDIR)/obj

CC=gcc
CFLAGS=-I$(IDIR)

_DEPS = display.h error.h lexer.h token.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = display.o error.o lexer.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

MAIN=$(ODIR)/main.o

#this is the VENT Transpiler executable
tvt: $(OBJ) $(MAIN)
	$(CC) -o $@ $> $^ $(CLFAGS) -g

$(ODIR):
	mkdir -p $@
 
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) -g

$(MAIN) : main.c
	$(CC) -c -o $@ $< $(CFLAGS) -g

.PHONY: clean debug help

help:
	@echo "call 'make tvt' or just 'make'"

clean:
	rm -fr $(ODIR)
	rm tvt

debug:
	@echo $(DEPS)
	@echo $(OBJ)

