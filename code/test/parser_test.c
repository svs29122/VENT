#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "valgrind.h"
#include "cutest.h"
#include "lexer.h"
#include "parser.h"
#include "display.h"

static void setup(char* in){
	InitLexer(in);
	InitParser();
}

void TestParseProgram_UseDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all;");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	

	struct UseStatement* stmt = (struct UseStatement*)prog->useStatements->block;
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_EntityDeclarationNoPorts(CuTest *tc){
	char* input = strdup("ent ander {\n}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_UseWithEntityDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all;\n\nent ander {\n}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct UseStatement* stmt = (struct UseStatement*)prog->useStatements->block;
	if(stmt != NULL)
		CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_EntityDeclarationWithPorts(CuTest *tc){
	char* input = strdup("ent ander{ a -> stl; b -> stl; y <- stl;}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_UseEntityWithPorts(CuTest *tc){
	char* input = strdup(" \
use ieee.std_logic_1164.all; \
ent ander { \
a -> stl; \
b -> stl; \
y <- stl; \
} \
");

	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct UseStatement* stmt = (struct UseStatement*)prog->useStatements->block;
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, unit->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", unit->as.entity.name->value);

	CuAssertPtrNotNullMsg(tc,"Port list NULL!", unit->as.entity.ports);	
	Dba* ports = unit->as.entity.ports;
	
	struct PortDecl* port1 = (struct PortDecl*)ports->block;
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "a", port1->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port1->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port1->dtype->value);

	struct PortDecl* port2 = (struct PortDecl*)(ports->block + ports->blockSize);
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "b", port2->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port2->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port2->dtype->value);

	struct PortDecl* port3 = (struct PortDecl*)(ports->block + (ports->blockSize * 2));
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "y", port3->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "<-", port3->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port3->dtype->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureDeclarationEmpty(CuTest *tc){
	char* input = strdup("arch behavioral(ander) {}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalDeclaration(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl;}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Arch body declarations NULL!", unit->as.architecture.declarations);		
	struct Declaration* decl = (struct Declaration*)unit->as.architecture.declarations->block;
	CuAssertIntEquals_Msg(tc,"Expected signal declaration!", SIGNAL_DECLARATION, decl->type);
		
	struct SignalDecl* sDecl = (struct SignalDecl*)&(decl->as.signalDeclaration);

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalInit(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl := '0';}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Signal declarations NULL!", unit->as.architecture.declarations);		
	struct Declaration* decl = (struct Declaration*)unit->as.architecture.declarations->block;
	CuAssertIntEquals_Msg(tc,"Expected signal declaration!", SIGNAL_DECLARATION, decl->type);
		
	struct SignalDecl* sDecl = (struct SignalDecl*)&(decl->as.signalDeclaration);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	CuAssertPtrNotNullMsg(tc,"Signal initialization expression NULL!", sDecl->expression);		
	CuAssertStrEquals_Msg(tc,"Expression not to char literal!", "0", ((struct CharExpr*)(sDecl->expression))->literal);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssign(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= '0';}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Arch declarations NULL!", unit->as.architecture.declarations);		
	struct Declaration* decl = (struct Declaration*)unit->as.architecture.declarations->block;
	CuAssertIntEquals_Msg(tc,"Expected signal declaration!", SIGNAL_DECLARATION, decl->type);
		
	struct SignalDecl* sDecl = (struct SignalDecl*)&(decl->as.signalDeclaration);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	CuAssertPtrNotNullMsg(tc,"Arch statements NULL!", unit->as.architecture.statements);
	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*)unit->as.architecture.statements->block;
	CuAssertIntEquals_Msg(tc,"Expected signal assignment statement!", SIGNAL_ASSIGNMENT, cstmt->type);

	struct SignalAssign* sAssign = &(cstmt->as.signalAssignment);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	CuAssertStrEquals_Msg(tc,"Expression not char literal!", "0", ((struct CharExpr*)(sAssign->expression))->literal);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssignBinaryExpression(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= a and b;}");
	setup(input);

	struct Program* prog = ParseProgram();

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;

	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	CuAssertPtrNotNullMsg(tc,"Arch declarations NULL!", unit->as.architecture.declarations);		
	struct Declaration* decl = (struct Declaration*)unit->as.architecture.declarations->block;
	CuAssertIntEquals_Msg(tc,"Expected signal declaration!", SIGNAL_DECLARATION, decl->type);
		
	struct SignalDecl* sDecl = (struct SignalDecl*)&(decl->as.signalDeclaration);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);

	CuAssertPtrNotNullMsg(tc,"Arch statements NULL!", unit->as.architecture.statements);
	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*)unit->as.architecture.statements->block;
	CuAssertIntEquals_Msg(tc,"Expected signal assignment statement!", SIGNAL_ASSIGNMENT, cstmt->type);

	struct SignalAssign* sAssign = &(cstmt->as.signalAssignment);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, sAssign->expression->type);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_EntityWithArchitecture(CuTest *tc){
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

	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct UseStatement* stmt = (struct UseStatement*)prog->useStatements->block;
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", stmt->value);

	Dba* arr = prog->units;
	struct DesignUnit* ent = (struct DesignUnit*)arr->block;
	CuAssertIntEquals_Msg(tc,"Expected ENTITY design unit!",  ENTITY, ent->type);
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", ent->as.entity.name->value);

	CuAssertPtrNotNullMsg(tc,"Port list NULL!", ent->as.entity.ports);	
	Dba* ports = ent->as.entity.ports;
	
	struct PortDecl* port1 = (struct PortDecl*)ports->block;
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "a", port1->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port1->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port1->dtype->value);

	struct PortDecl* port2 = (struct PortDecl*)(ports->block + ports->blockSize);
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "b", port2->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port2->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port2->dtype->value);

	struct PortDecl* port3 = (struct PortDecl*)(ports->block + (ports->blockSize * 2));
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "y", port3->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "<-", port3->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port3->dtype->value);

	struct DesignUnit* unit = (struct DesignUnit*)(arr->block + arr->blockSize);
	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertPtrNotNullMsg(tc,"Arch declarations NULL!", unit->as.architecture.declarations);		

	struct Declaration* decl = (struct Declaration*)unit->as.architecture.declarations->block;
	CuAssertIntEquals_Msg(tc,"Expected signal declaration!", SIGNAL_DECLARATION, decl->type);
		
	struct SignalDecl* sDecl = (struct SignalDecl*)&(decl->as.signalDeclaration);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sDecl->name->value);
	CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", "stl", sDecl->dtype->value);
	CuAssertPtrNotNullMsg(tc,"Signal initialization expression NULL!", sDecl->expression);		
	CuAssertStrEquals_Msg(tc,"Expression not to char literal!", "0", ((struct CharExpr*)(sDecl->expression))->literal);

	CuAssertPtrNotNullMsg(tc,"Arch statements NULL!", unit->as.architecture.statements);
	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*)unit->as.architecture.statements->block;
	CuAssertIntEquals_Msg(tc,"Expected signal assignment statement!", SIGNAL_ASSIGNMENT, cstmt->type);

	struct SignalAssign* sAssign = &(cstmt->as.signalAssignment);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, sAssign->expression->type);

	cstmt = (struct ConcurrentStatement*)(unit->as.architecture.statements->block + unit->as.architecture.statements->blockSize);
	sAssign = &(cstmt->as.signalAssignment);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "y", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		
	CuAssertStrEquals_Msg(tc,"Expression not char literal!", "temp", ((struct Identifier*)(sAssign->expression))->value);

	//PrintProgram(prog);

	FreeProgram(prog);	
	free(input);
}

#include <time.h>
void TestParseProgram_LoopedProgramParsing(CuTest *tc){
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

	float timeA = (float)clock()/CLOCKS_PER_SEC;
	for(int i = 0; i < 10000; i++){
		setup(input);
		
		struct Program* prog = ParseProgram();
		
		CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
		CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
		CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

		FreeProgram(prog);	

		//break early if runing on Valgrind
		if(RUNNING_ON_VALGRIND) break;
	}
	float timeB = (float)clock()/CLOCKS_PER_SEC;
	
	//Note: on an Intel Core i7-7700 @3.60GHz this takes ~0.128877 seconds
	if((timeB-timeA) > 1.0){
		printf("Parsing 10,000 VENT programs took longer than a second: ~%fs\r\n", timeB-timeA);
	}

	free(input);
}

void TestParseProgram_ProcessStatement(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(ander){ \
			sig a stl := '0'; \
			sig b stl; \
			sig y stl := '0'; \
			\
			proc (clk) { \
					y <= a and b; \
			} \
		} \
	");
	setup(input);

	struct Program* prog = ParseProgram();
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*)unit->as.architecture.statements->block;
	CuAssertIntEquals_Msg(tc,"Expected Process statement!", PROCESS, cstmt->type);

	struct Process* proc = (struct Process*)(&(cstmt->as.process));
	CuAssertStrEquals_Msg(tc,"Expected clk signal in sensitivity list!", "clk", proc->sensitivityList->value);
	
	struct SequentialStatement* qstmt = (struct SequentialStatement*)proc->statements->block;
	CuAssertIntEquals_Msg(tc,"Expected signal assignment in process body!", SEQ_SIGNAL_ASSIGNMENT, qstmt->type);

	struct SignalAssign* sAssign = (struct SignalAssign*)(&(qstmt->as.signalAssignment));
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "y", sAssign->target->value);
	CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		

	struct BinaryExpr* bExp = (struct BinaryExpr*) sAssign->expression;
	CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, bExp->self.type);
	CuAssertIntEquals_Msg(tc,"Expected identifier!", NAME_EXPR, ((struct Identifier*)bExp->left)->self.type);
	CuAssertIntEquals_Msg(tc,"Expected identifier!", NAME_EXPR, ((struct Identifier*)bExp->right)->self.type);
	CuAssertStrEquals_Msg(tc,"Expected and operation!", "and", bExp->op);

	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithDeclarations(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(ander){ \n \
			sig u stl := '0'; \n \
			\n \
			proc () { \n \
				sig s stl := '0'; \n \
				var i int := 123456; \n \
				\n \
			} \n \
		} \n \
	");
	setup(input);

	struct Program* prog = ParseProgram();
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*)unit->as.architecture.statements->block;
	CuAssertIntEquals_Msg(tc,"Expected Process statement!", PROCESS, cstmt->type);

	struct Process* proc = (struct Process*)(&(cstmt->as.process));

	PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithWhileWait(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(ander){ \
			\
			proc () { \
				sig s stl := '0'; \
				var i int; \
				\
				while(i < 10) { \
					i := i + 2; \
				} \
				wait; \
			} \
		} \
	");
	setup(input);

	struct Program* prog = ParseProgram();
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*)prog->units->block;
	CuAssertIntEquals_Msg(tc,"Expected ARCH design unit!",  ARCHITECTURE, unit->type);
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", unit->as.architecture.archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", unit->as.architecture.entName->value);

	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*)unit->as.architecture.statements->block;
	CuAssertIntEquals_Msg(tc,"Expected Process statement!", PROCESS, cstmt->type);

	struct Process* proc = (struct Process*)(&(cstmt->as.process));

	PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParse_(CuTest *tc){
	char* input = strdup(" \
		\
	");
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
	SUITE_ADD_TEST(suite, TestParseProgram_LoopedProgramParsing);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessStatement);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithDeclarations);
	SUITE_ADD_TEST(suite, TestParse_);

	return suite;
}

