# Makefile for building and running UnitTests
#
# NOTE:
# 	- running "clean" target will clean up all test objects, but leave behind app objects
# 		~ e.g. lexer_test.o will be cleaned but lexer.o will stick around


IDIR=../inc
SDIR=../src
TDIR=.
SODIR=$(SDIR)/obj
TODIR=$(TDIR)/obj

CC=gcc
CFLAGS=-I$(IDIR) -g

_DEP = parser.h ast.h lexer.h token.h cutest.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEP))

_OBJ = parser.o lexer.o
OBJS = $(patsubst %,$(SODIR)/%,$(_OBJ))

_TOBJ = parser_test.o lexer_test.o all_tests.o cutest.o
TOBJS = $(patsubst %,$(TODIR)/%,$(_TOBJ))

# this is the executable to run all tests
allTests: $(TOBJS) $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)

#build source code
$(SODIR):
	mkdir -p $@

$(SODIR)/%.o: $(SDIR)/%.c $(DEPS) | $(SODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

#build test code
$(TODIR):
	mkdir -p $@

$(TODIR)/%.o: $(TDIR)/%.c $(DEPS) | $(TODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean print

clean:
	rm -fr $(TODIR)
	rm -fr $(SODIR)
	rm -f allTests

print:
	@echo $(TDIR)
	@echo $(TODIR)
	@echo $(TOBJS)
