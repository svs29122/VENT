#simple makefile for building VENT

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

$(ODIR):
	mkdir -p $@
 
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(MAIN) : main.c
	$(CC) -c -o $@ $< $(CFLAGS)


tvt: $(OBJ) $(MAIN)
	$(CC) -o $@ $> $^ $(CLFAGS)


.PHONY: clean debug

clean:
	rm -fr $(ODIR)
	rm tvt

debug:
	@echo $(DEPS)
	@echo $(OBJ)

