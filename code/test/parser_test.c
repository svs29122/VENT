#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cutest.h"
#include "lexer.h"
#include "parser.h"

void setup(char* in){
	InitLexer(in);
	InitParser();
}

void TestParseProgram_EntityDeclarationNoPorts(CuTest *tc){
	char* input = strdup("ent ander {\n}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, prog->units->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", prog->units->decl.entity.name->value);
	CuAssertPtrEquals_Msg(tc,"Port Declaration not NULL!", NULL, prog->units->decl.entity.ports);
	
	free(input);
}

void TestParse_(CuTest *tc){
	char* input = strdup("ent ander {}");
	setup(input);

	
	free(input);
}

CuSuite* ParserTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestParseProgram_EntityDeclarationNoPorts);
	SUITE_ADD_TEST(suite, TestParse_);

	return suite;
}

