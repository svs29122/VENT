#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cutest.h"
#include "lexer.h"
#include "parser.h"
#include "display.h"
#include "transpile.h"

static void setup(char* in){
	InitLexer(in);
	InitParser();
}

void TestTranspileProgram_Simple(CuTest *tc){
	char* fileName = strdup("ander.vent");
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent ander { \
			a -> stl; \
			b -> stl; \
			y <- stl; \
		} \
		\
		arch behavioral(ander){ \
			sig temp stl := '0'; \
		\
			temp <= a and b; \
			y <= temp; \
			} \
		");
	setup(input);

	struct Program* prog = ParseProgram();

	TranspileProgram(prog, fileName);

	FreeProgram(prog);
	free(input);
	free(fileName);
}

CuSuite* TranspileTestGetSuite(){

	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestTranspileProgram_Simple);

	return suite;
}

