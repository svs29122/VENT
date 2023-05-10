#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <lexer.h>
#include <parser.h>
#include <display.h>
#include <emitter.h>

#include "cutest.h"

void TestTranspileProgram_Simple(CuTest *tc){
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

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithProcess(CuTest *tc){
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
			proc(){ \
				var count int := 1; \
				while( i < 10 ) { \
					i := i + 1; \
				} \
			} \
		} \
		");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

CuSuite* TranspileTestGetSuite(){

	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestTranspileProgram_Simple);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithProcess);

	return suite;
}

