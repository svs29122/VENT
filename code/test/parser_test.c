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

void TestParseProgram_UseDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	

	UseStatement* stmt = (UseStatement*)prog->useStatements->block;
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	FreeProgram(prog);	
	free(input);
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
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_UseWithEntityDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all\n\nent ander {\n}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	UseStatement* stmt = (UseStatement*)prog->useStatements->block;
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, prog->units->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", prog->units->decl.entity.name->value);
	CuAssertPtrEquals_Msg(tc,"Port Declaration not NULL!", NULL, prog->units->decl.entity.ports);

	PrintProgram(prog);
	
	FreeProgram(prog);	
	free(input);
}

void TestParse_(CuTest *tc){
	char* input = strdup("ent ander {}");
	setup(input);

	
	free(input);
}

CuSuite* ParserTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestParseProgram_UseDeclaration);
	SUITE_ADD_TEST(suite, TestParseProgram_EntityDeclarationNoPorts);
	SUITE_ADD_TEST(suite, TestParseProgram_UseWithEntityDeclaration);
	SUITE_ADD_TEST(suite, TestParse_);

	return suite;
}

