# Makefile for building parser
#
# Notes:
#			parser object files get linked together into a single unit 'parser_mod.o'
#

IDIR=../../inc
SDIR=./

LODIR?=$(SDIR)/obj

ODIR?=$(SDIR)../obj
DDIR=$(SDIR)../obj_d

CC=gcc
CFLAGS=-I$(IDIR)
DFLAGS= $(CFLAGS) -g -O0 -DDEBUG

_DEP = parser.h ast.h dba.h lexer.h token.h error.h util.h expression.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEP))

_OBJ = parser.o free.o error.o utils.o expression.o
OBJ ?= $(patsubst %,$(LODIR)/%,$(_OBJ))

OBJ_FINAL = $(ODIR)/parser_mod.o

all: $(OBJ_FINAL)

$(OBJ_FINAL): $(OBJ) | $(ODIR)
	ld -r $^ -o $@

$(LODIR):
	mkdir -p $@

$(ODIR):
	mkdir -p $@

$(DDIR):
	mkdir -p $@

$(LODIR)/%.o: %.c | $(LODIR)
	$(CC) -c $^ $(CFLAGS) -o $@ 
 
.PHONY: clean all

debug: 
	$(MAKE) all CFLAGS="$(DFLAGS)" ODIR=$(DDIR)

clean:
	rm -rf $(LODIR)
	rm -f $(ODIR)/parser_mod.o

cleand:
	$(MAKE) clean ODIR=$(DDIR)
