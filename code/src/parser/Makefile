# Makefile for building parser
#
# Notes:
#	i) The parser was getting huge so it was broken up
#		- instead of breaking it up into separate modules each with its own api (*.h), the 
#			related bits of the parser (including implementation) were moved to header files
#			that get included in parser.c
#		- these header files include static functions and static data used only by the parser.
#			the result is a single translation unit made up of modular headers organized by 
#			function. while some may frown on this, IMO it leads to the cleanest solution 
#			while maintaing the the parser as a single module
#		- the public api for the parser itself (parser.h) is still located in the global 
#			include directory "inc" 
#

IDIR=../../inc
SDIR=./
ODIR?=$(SDIR)../obj
DDIR=$(SDIR)../obj_d

CC=gcc
CFLAGS=-I$(IDIR)
DFLAGS= $(CFLAGS) -g -O0 -DDEBUG

_DEP = parser.h ast.h dba.h lexer.h token.h error.h free.h util.h expression.h 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEP))

_OBJ = parser.o
OBJ ?= $(patsubst %,$(ODIR)/%,$(_OBJ))

all: $(OBJ)

$(ODIR):
	mkdir -p $@

$(ODIR)/parser.o: parser.c | $(ODIR)
	$(CC) -c $^ $(CFLAGS) -o $@ 
 
.PHONY: clean all

debug: 
	$(MAKE) all CFLAGS="$(DFLAGS)" ODIR=$(DDIR)

clean:
	rm $(ODIR)/parser.o

cleand:
	$(MAKE) clean ODIR=$(DDIR)
