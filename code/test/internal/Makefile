# Makefile for building and running Internal Unit Tests
#
# NOTE:
#

IDIR=../../inc
SDIR=../../src

TSDIR=..

WDIR=.

SODIR=$(SDIR)/obj
TODIR=$(TSDIR)/obj
IODIR=$(WDIR)/obj

CC=gcc
CFLAGS=-I$(IDIR) -g

_DEP = parser.h ast.h dba.h token.h display.h emitter.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEP))

_OBJ = dba.o lexer.o display.o ast.o emitter.o
OBJS = $(patsubst %,$(SODIR)/%,$(_OBJ))

_TOBJ = cutest.o
TOBJS = $(patsubst %,$(TODIR)/%,$(_TOBJ))

_PIOBJ = parser_internals.o 
PIOBJS = $(patsubst %,$(IODIR)/%,$(_PIOBJ))

# this is internal tests just for the parser
ParserInternalTests: $(PIOBJS) $(OBJS) $(TOBJS) $(DEPS) 
	$(CC) $^ $(CFLAGS) -o $@

#build source code
$(SODIR):
	mkdir -p $@

$(SODIR)/%.o: $(SDIR)/%.c $(DEPS) | $(SODIR)
	$(CC) -c $< $(CFLAGS) -o $@

$(TODIR):
	mkdir -p $@

$(TODIR)/%.o: $(TSDIR)/%.c $(DEPS) | $(TODIR)
	$(CC) -c $< $(CFLAGS) -o $@

$(IODIR):
	mkdir -p $@

$(IODIR)/%.o: $(WDIR)/%.c $(DEPS) | $(IODIR)
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY: runtest clean print

clean:
	rm -fr $(IODIR)
	rm -fr $(TODIR)
	rm -fr $(SODIR)
	rm -f ParserInternalTests

printo:
	@echo $(OBJS)
	@echo $(TOBJS)
	@echo $(PIOBJS)

printd:
	@echo "Source Dirs:"
	@echo $(IDIR)
	@echo $(SDIR)
	@echo "\nTest Dirs:"
	@echo $(TSDIR)
	@echo $(WDIR)
	@echo "\nOutput Dirs:"
	@echo $(SODIR)
	@echo $(TODIR)
	@echo $(IODIR)

