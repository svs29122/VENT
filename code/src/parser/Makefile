# Makefile for building and running UnitTests
#
# NOTE:
# 	- running "clean" target will clean up all test objects, but leave behind app objects
# 		~ e.g. lexer_test.o will be cleaned but lexer.o will stick around


IDIR=../../inc
SDIR=./
ODIR?=$(SDIR)../obj
DDIR=$(SDIR)../obj_d

CC=gcc
CFLAGS=-I$(IDIR)
DFLAGS= $(CFLAGS) -g -O0 -DDEBUG

_DEP = parser.h ast.h dba.h lexer.h token.h error.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEP))

_OBJ = error.o parser.o
OBJ ?= $(patsubst %,$(ODIR)/%,$(_OBJ))

all: $(OBJ)

$(ODIR):
	mkdir -p $@

$(ODIR)/parser.o: parser.c | $(ODIR)
	$(CC) -c $^ $(CFLAGS) -o $@ 
 
$(ODIR)/error.o: error.c | $(ODIR)
	$(CC) -c $^ $(CFLAGS) -o $@

.PHONY: clean all

debug: 
	$(MAKE) all CFLAGS="$(DFLAGS)" ODIR=$(DDIR)

clean:
	rm $(ODIR)/parser.o
	rm $(ODIR)/error.o

cleand:
	$(MAKE) clean ODIR=$(DDIR)
