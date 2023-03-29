#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cutest.h"
#include "lexer.h"
#include "parser.h"
#include "display.h"

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

	DesignUnit* unit = (DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);
	
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
	if(stmt != NULL)
		CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	DesignUnit* unit = (DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_EntityDeclarationWithPorts(CuTest *tc){
	char* input = strdup("ent ander{ a -> stl; b -> stl; y <- stl;}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	DesignUnit* unit = (DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_UseEntityWithPorts(CuTest *tc){
	char* input = strdup(" \
use ieee.std_logic_1164.all \
ent ander { \
a -> stl; \
b -> stl; \
y <- stl; \
} \
");

	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	DesignUnit* unit = (DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);

	//TODO: you aren't validating any port stuff here. You need to fix this! 

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureDeclarationEmpty(CuTest *tc){
	char* input = strdup("arch behavioral(ander) {}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	DesignUnit* unit = (DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalDeclaration(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl;}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	DesignUnit* unit = (DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Signal declarations NULL!", unit->as.architecture.declarations);		
	SignalDecl* sDecl = (SignalDecl*)unit->as.architecture.declarations->block;

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalInit(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl := '0';}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	DesignUnit* unit = (DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Signal declarations NULL!", unit->as.architecture.declarations);		
	SignalDecl* sDecl = (SignalDecl*)unit->as.architecture.declarations->block;

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	CuAssertPtrNotNullMsg(tc,"Signal initialization expression NULL!", sDecl->expression);		
	CuAssertStrEquals_Msg(tc,"Expression not to char literal!", "0", ((CharExpr*)(sDecl->expression))->literal);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssign(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= '0';}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	DesignUnit* unit = (DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Arch declarations NULL!", unit->as.architecture.declarations);		
	SignalDecl* sDecl = (SignalDecl*)unit->as.architecture.declarations->block;

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	CuAssertPtrNotNullMsg(tc,"Arch statements NULL!", unit->as.architecture.statements);
	SignalAssign* sAssign = (SignalAssign*)unit->as.architecture.statements->block;

	
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	CuAssertStrEquals_Msg(tc,"Expression not char literal!", "0", ((CharExpr*)(sAssign->expression))->literal);

	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssignBinaryExpression(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= a and b;}");
	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	DesignUnit* unit = (DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Arch declarations NULL!", unit->as.architecture.declarations);		
	SignalDecl* sDecl = (SignalDecl*)unit->as.architecture.declarations->block;

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	CuAssertPtrNotNullMsg(tc,"Arch statements NULL!", unit->as.architecture.statements);

	SignalAssign* sAssign = (SignalAssign*)unit->as.architecture.statements->block;
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, sAssign->expression->type);

	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_EntityWithArchitecture(CuTest *tc){
	char* input = strdup(" \
use ieee.std_logic_1164.all \
ent ander { \
a -> stl; \
b -> stl; \
y <- stl; \
} \
arch behavioral(ander){ \
sig temp stl := '0'; \
temp <= a and b; \
y <= temp; \
} \
");

	setup(input);

	Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	UseStatement* stmt = (UseStatement*)prog->useStatements->block;
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	Dba* arr = prog->units;
	DesignUnit* ent = (DesignUnit*)arr->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, ent->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", ent->as.entity.name->value);

	DesignUnit* unit = (DesignUnit*)(arr->block + arr->blockSize);
	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertPtrNotNullMsg(tc,"Arch declarations NULL!", unit->as.architecture.declarations);		

	SignalDecl* sDecl = (SignalDecl*)unit->as.architecture.declarations->block;
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);
	CuAssertPtrNotNullMsg(tc,"Signal initialization expression NULL!", sDecl->expression);		
	CuAssertStrEquals_Msg(tc,"Expression not to char literal!", "0", ((CharExpr*)(sDecl->expression))->literal);

	CuAssertPtrNotNullMsg(tc,"Arch statements NULL!", unit->as.architecture.statements);

	SignalAssign* sAssign = (SignalAssign*)unit->as.architecture.statements->block;
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, sAssign->expression->type);

	//sAssign = (SignalAssign*)unit->as.architecture.statements->block + unit->as.architecture.statements->blockSize;
	//CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "y", sAssign->target->value);
	//CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	//CuAssertStrEquals_Msg(tc,"Expression not char literal!", "temp", ((Identifier*)(sAssign->expression))->value);

	PrintProgram(prog);

	FreeProgram(prog);	
	free(input);
}

void TestParse_(CuTest *tc){
	char* input = strdup("ent ander {}");
	//setup(input);

	free(input);
}

CuSuite* ParserTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestParseProgram_UseDeclaration);
	SUITE_ADD_TEST(suite, TestParseProgram_EntityDeclarationNoPorts);
	SUITE_ADD_TEST(suite, TestParseProgram_UseWithEntityDeclaration);
	SUITE_ADD_TEST(suite, TestParseProgram_EntityDeclarationWithPorts);
	SUITE_ADD_TEST(suite, TestParseProgram_UseEntityWithPorts);
	SUITE_ADD_TEST(suite, TestParseProgram_ArchitectureDeclarationEmpty);
	SUITE_ADD_TEST(suite, TestParseProgram_ArchitectureWithSignalDeclaration);
	SUITE_ADD_TEST(suite, TestParseProgram_ArchitectureWithSignalInit);
	SUITE_ADD_TEST(suite, TestParseProgram_ArchitectureWithSignalAssign);
	SUITE_ADD_TEST(suite, TestParseProgram_ArchitectureWithSignalAssignBinaryExpression);
	SUITE_ADD_TEST(suite, TestParseProgram_EntityWithArchitecture);
	SUITE_ADD_TEST(suite, TestParse_);

	return suite;
}

