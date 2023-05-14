#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <parser.h>
#include <ast.h>
#include <display.h>

#include "valgrind.h"
#include "cutest.h"

//helper functions
static void checkProgram(CuTest* tc, struct Program* prog){
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
}

static void checkUse(CuTest* tc, struct Program* prog, const char* useLiteral){
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	struct UseStatement* stmt = (struct UseStatement*) ReadBlockArray(prog->useStatements, 0);
	CuAssertStrEquals_Msg(tc,"use path incorrect!", useLiteral, stmt->value);

}

static void checkDesignUnit(CuTest* tc, struct Program* prog, int dnum, int type, const char* uname, const char* ename){
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*) ReadBlockArray(prog->units, dnum-1);
	CuAssertIntEquals_Msg(tc,"Wrong design unit type!",  type, unit->type);
	if(type == ENTITY){
		CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", uname, unit->as.entity.name->value);
	} else if (type == ARCHITECTURE){
		CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", uname, unit->as.architecture.archName->value);
		CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", ename, unit->as.architecture.entName->value);
	}
}

static void checkPort(CuTest* tc, struct Program* prog, int dnum, int pnum, const char* id, const char* mode, const char* dtype){

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	

	struct DesignUnit* unit = (struct DesignUnit*) ReadBlockArray(prog->units, dnum-1);
	Dba* ports = unit->as.entity.ports;
	
	struct PortDecl* port = (struct PortDecl*) ReadBlockArray(ports, pnum-1);
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", id, port->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", mode, port->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", dtype, port->dtype->value);
}

static void checkArchDeclaration(CuTest* tc, struct Program* prog, int dnum, int declType, int declnum, const char* name1, const char* name2, const char* name3){

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	struct DesignUnit* unit = (struct DesignUnit*) ReadBlockArray(prog->units, dnum-1);

	CuAssertPtrNotNullMsg(tc,"Arch body declarations NULL!", unit->as.architecture.declarations);		
	Dba* declarations = unit->as.architecture.declarations;

	struct Declaration* decl = (struct Declaration*) ReadBlockArray(declarations, declnum-1);

	switch (declType) { 
		case SIGNAL_DECLARATION: {
			CuAssertIntEquals_Msg(tc,"Expected signal declaration!", SIGNAL_DECLARATION, decl->type);		
			struct SignalDecl* sDecl = (struct SignalDecl*)&(decl->as.signalDeclaration);

			CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", name1, sDecl->name->value);
			CuAssertStrEquals_Msg(tc,"Signal data type incorrect!", name2, sDecl->dtype->value);
			
			if(sDecl->expression){
				CuAssertPtrNotNullMsg(tc,"Signal initialization expression NULL!", sDecl->expression);		
				CuAssertStrEquals_Msg(tc,"Expression not to char literal!", name3, ((struct CharExpr*)(sDecl->expression))->literal);
			}
			break;
		}

		default:
			break;
	}

}

static void checkConcurrentStatement(CuTest* tc, struct Program* prog, int dnum, int stmtType, int snum, const char* name1, const char* name2, const char* name3){

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	struct DesignUnit* unit = (struct DesignUnit*) ReadBlockArray(prog->units, dnum-1);

	CuAssertPtrNotNullMsg(tc,"Arch body statements NULL!", unit->as.architecture.statements);		
	Dba* statements = unit->as.architecture.statements;

	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*) ReadBlockArray(statements, snum-1);

	switch (stmtType) { 
		case SIGNAL_ASSIGNMENT: {
			CuAssertIntEquals_Msg(tc,"Expected signal assignment statement!", SIGNAL_ASSIGNMENT, cstmt->type);
	
			struct SignalAssign* sAssign = &(cstmt->as.signalAssignment);
			CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", name1, sAssign->target->value);

			if(sAssign->expression){
				CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		

				switch (sAssign->expression->type) {

					case BINARY_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, sAssign->expression->type);
						break;
					}
					
					case CHAR_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected char expression!", CHAR_EXPR, sAssign->expression->type);
						CuAssertStrEquals_Msg(tc,"Expression not char literal!", name2, ((struct CharExpr*)(sAssign->expression))->literal);
						break;
					}

					case NAME_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected char expression!", NAME_EXPR, sAssign->expression->type);
						CuAssertStrEquals_Msg(tc,"Expression not char literal!", name2, ((struct Identifier*)(sAssign->expression))->value);
						break;
					}

					default:
						break;
				}
			}
			break;
		}

		case PROCESS: {
			CuAssertIntEquals_Msg(tc,"Expected process statement!", PROCESS, cstmt->type);
		
			struct Process* proc = &(cstmt->as.process);
	
			if(proc->sensitivityList){
				CuAssertStrEquals_Msg(tc,"Sensitivity List mismatch", name1, ((struct Identifier*)(proc->sensitivityList))->value);
			}
			break;
		}

		default:
			break;
	}

}

static void checkSequentialStatement(CuTest* tc, struct Program* prog, int duNum,  int csNum, int qsNum, int qstmtType, const char* name1, const char* name2, const char* name3){

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	struct DesignUnit* unit = (struct DesignUnit*) ReadBlockArray(prog->units, duNum-1);
	struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*) ReadBlockArray(unit->as.architecture.statements, csNum-1);
	struct Process* proc = (struct Process*)(&(cstmt->as.process));
	struct SequentialStatement* qstmt = (struct SequentialStatement*) ReadBlockArray(proc->statements, qsNum-1);

	switch (qstmtType) { 
		case QSIGNAL_ASSIGNMENT: {
			CuAssertIntEquals_Msg(tc,"Expected signal assignment statement!", QSIGNAL_ASSIGNMENT, cstmt->type);
	
			struct SignalAssign* sAssign = &(qstmt->as.signalAssignment);
			CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", name1, sAssign->target->value);

			if(sAssign->expression){
				CuAssertPtrNotNullMsg(tc,"Signal assignment expression NULL!", sAssign->expression);		

				switch (sAssign->expression->type) {

					case BINARY_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, sAssign->expression->type);
						break;
					}
					
					case CHAR_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected char expression!", CHAR_EXPR, sAssign->expression->type);
						CuAssertStrEquals_Msg(tc,"Expression not char literal!", name2, ((struct CharExpr*)(sAssign->expression))->literal);
						break;
					}

					case NAME_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected char expression!", NAME_EXPR, sAssign->expression->type);
						CuAssertStrEquals_Msg(tc,"Expression not char literal!", name2, ((struct Identifier*)(sAssign->expression))->value);
						break;
					}

					default:
						break;
				}
			}
			break;
		}

		case VARIABLE_ASSIGNMENT: {
			CuAssertIntEquals_Msg(tc,"Expected variable assignment statement!", VARIABLE_ASSIGNMENT, qstmt->type);
		
			struct VariableAssign* vAssign = &(qstmt->as.variableAssignment);
			CuAssertStrEquals_Msg(tc,"Variable identifier incorrect!", name1, vAssign->target->value);

			if(vAssign->expression){
				CuAssertPtrNotNullMsg(tc,"Variable assignment expression NULL!", vAssign->expression);		

				switch (vAssign->expression->type) {

					case BINARY_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected binary expression!", BINARY_EXPR, vAssign->expression->type);
						break;
					}
					
					case CHAR_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected char expression!", CHAR_EXPR,vAssign->expression->type);
						CuAssertStrEquals_Msg(tc,"Expression not char literal!", name2, ((struct CharExpr*)(vAssign->expression))->literal);
						break;
					}

					case NAME_EXPR: {
						CuAssertIntEquals_Msg(tc,"Expected char expression!", NAME_EXPR, vAssign->expression->type);
						CuAssertStrEquals_Msg(tc,"Expression not char literal!", name2, ((struct Identifier*)(vAssign->expression))->value);
						break;
					}

					default:
						break;
				}
			}
			break;
		}

		case WHILE_STATEMENT: {
			CuAssertIntEquals_Msg(tc,"Expected while loop!", WHILE_STATEMENT, qstmt->type);
		}
	
		default:
			break;
	}

}

// the actual tests
void TestParseProgram_UseDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all;");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkUse(tc, prog, "ieee.std_logic_1164.all");

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_EntityDeclarationNoPorts(CuTest *tc){
	char* input = strdup("ent ander {\n}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkDesignUnit(tc, prog, 1, ENTITY, "ander", NULL);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_UseWithEntityDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all;\n\nent ander {\n}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkUse(tc, prog, "ieee.std_logic_1164.all");
	checkDesignUnit(tc, prog, 1, ENTITY, "ander" , NULL);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_EntityDeclarationWithPorts(CuTest *tc){
	char* input = strdup("ent ander{ a -> stl; b -> stl; y <- stl;}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkDesignUnit(tc, prog, 1, ENTITY, "ander", NULL);

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

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkUse(tc, prog, "ieee.std_logic_1164.all");
	checkDesignUnit(tc, prog, 1, ENTITY, "ander", NULL);

	checkPort(tc, prog, 1, 1, "a", "->", "stl");
	checkPort(tc, prog, 1, 2, "b", "->", "stl");
	checkPort(tc, prog, 1, 3, "y", "<-", "stl");

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureDeclarationEmpty(CuTest *tc){
	char* input = strdup("arch behavioral(ander) {}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkDesignUnit(tc, prog, 1, ARCHITECTURE, "behavioral", "ander");
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalDeclaration(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl;}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkDesignUnit(tc, prog, 1, ARCHITECTURE, "behavioral", "ander");
	checkArchDeclaration(tc, prog, 1, SIGNAL_DECLARATION, 1, "temp", "stl", NULL);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalInit(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl := '0';}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkDesignUnit(tc, prog, 1, ARCHITECTURE, "behavioral", "ander");
	checkArchDeclaration(tc, prog, 1, SIGNAL_DECLARATION, 1, "temp", "stl", "0");

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssign(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= '0';}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkDesignUnit(tc, prog, 1, ARCHITECTURE, "behavioral", "ander");
	checkArchDeclaration(tc, prog, 1, SIGNAL_DECLARATION, 1, "temp", "stl", "0");
	checkConcurrentStatement(tc, prog, 1, SIGNAL_ASSIGNMENT, 1, "temp", "0", NULL);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssignBinaryExpression(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= a and b;}");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkDesignUnit(tc, prog, 1, ARCHITECTURE, "behavioral", "ander");
	checkArchDeclaration(tc, prog, 1, SIGNAL_DECLARATION, 1, "temp", "stl", NULL);
	checkConcurrentStatement(tc, prog, 1, SIGNAL_ASSIGNMENT, 1, "temp", NULL, NULL);

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

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	checkUse(tc, prog, "ieee.std_logic_1164.all");

	int unitNum = 1;
	int portNum = 1;
	checkDesignUnit(tc, prog, unitNum, ENTITY, "ander" , NULL);
	checkPort(tc, prog, unitNum, portNum++, "a", "->", "stl");
	checkPort(tc, prog, unitNum, portNum++, "b", "->", "stl");
	checkPort(tc, prog, unitNum, portNum++, "y", "<-", "stl");

	unitNum += 1;
	int declNum = 1;
	int stmtNum = 1;

	checkDesignUnit(tc, prog, unitNum, ARCHITECTURE, "behavioral", "ander");
	checkArchDeclaration(tc, prog, unitNum, SIGNAL_DECLARATION, declNum, "temp", "stl", "0");
	checkConcurrentStatement(tc, prog, unitNum, SIGNAL_ASSIGNMENT, declNum++, "temp", NULL, NULL);
	checkConcurrentStatement(tc, prog, unitNum, SIGNAL_ASSIGNMENT, declNum++, "y", "temp", NULL);

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
		
		struct Program* prog = ParseProgram(input);
		checkProgram(tc, prog);
		checkUse(tc, prog, "ieee.std_logic_1164.all");

		int unitNum = 1;
		int portNum = 1;
		checkDesignUnit(tc, prog, unitNum, ENTITY, "ander" , NULL);
		checkPort(tc, prog, unitNum, portNum++, "a", "->", "stl");
		checkPort(tc, prog, unitNum, portNum++, "b", "->", "stl");
		checkPort(tc, prog, unitNum, portNum++, "y", "<-", "stl");

		unitNum += 1;
		int declNum = 1;
		int stmtNum = 1;

		checkDesignUnit(tc, prog, unitNum, ARCHITECTURE, "behavioral", "ander");
		checkArchDeclaration(tc, prog, unitNum, SIGNAL_DECLARATION, declNum, "temp", "stl", "0");
		checkConcurrentStatement(tc, prog, unitNum, SIGNAL_ASSIGNMENT, stmtNum++, "temp", NULL, NULL);
		checkConcurrentStatement(tc, prog, unitNum, SIGNAL_ASSIGNMENT, stmtNum++, "y", "temp", NULL);

		FreeProgram(prog);	

		//break early if runing on Valgrind
		if(RUNNING_ON_VALGRIND) break;
	}
	float timeB = (float)clock()/CLOCKS_PER_SEC;
	
	//Note: on an Intel Core i7-7700 @3.60GHz this takes ~0.134527 seconds
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

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);

	int unitNum = 1;
	checkDesignUnit(tc, prog, unitNum, ARCHITECTURE, "behavioral", "ander");

	int declNum = 0;
	checkArchDeclaration(tc, prog, unitNum, SIGNAL_DECLARATION, ++declNum, "a", "stl", "0");
	checkArchDeclaration(tc, prog, unitNum, SIGNAL_DECLARATION, ++declNum, "b", "stl", NULL);
	checkArchDeclaration(tc, prog, unitNum, SIGNAL_DECLARATION, ++declNum, "y", "stl", "0");
	
	int stmtNum = 1;
	checkConcurrentStatement(tc, prog, unitNum, PROCESS, stmtNum, "clk", NULL, NULL);

	int qstmtNum = 1;
	checkSequentialStatement(tc, prog, unitNum, stmtNum, qstmtNum, QSIGNAL_ASSIGNMENT, "y", NULL, NULL);

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

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);

	int unitNum = 0;
	checkDesignUnit(tc, prog, ++unitNum, ARCHITECTURE, "behavioral", "ander");

	int declNum = 0;
	checkArchDeclaration(tc, prog, unitNum, SIGNAL_DECLARATION, ++declNum, "u", "stl", "0");
	
	int stmtNum = 0;
	checkConcurrentStatement(tc, prog, unitNum, PROCESS, ++stmtNum, NULL, NULL, NULL);

	//TODO: write code to check declarations in a process

	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithEmptyWhileWait(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(ander){\n \
			\n \
			proc () {\n \
				while() {\n \
				}\n \
				wait;\n \
			}\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);
	int unitNum = 0;
	checkDesignUnit(tc, prog, ++unitNum, ARCHITECTURE, "behavioral", "ander");
	int stmtNum = 0;
	checkConcurrentStatement(tc, prog, unitNum, PROCESS, ++stmtNum, "", NULL, NULL);
	int qstmtNum = 0;
	checkSequentialStatement(tc, prog, unitNum, stmtNum, ++qstmtNum, WHILE_STATEMENT, NULL, NULL, NULL);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithWhileLoop(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all;\n \
		\n \
		ent ander {\n \
			a -> stl;\n \
			b -> stl;\n \
			y <- stl;\n \
		}\n \
		\n \
		arch behavioral(ander){\n \
			a <= '0';\n \
			\n \
			proc () {\n \
				sig s stl := '0';\n \
				var i int;\n \
				\n \
				while(i < 10) {\n \
					s <= '1';\n \
					i := i+2;\n \
				}\n \
				wait;\n \
			}\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);

	int unitNum = 1;
	checkDesignUnit(tc, prog, ++unitNum, ARCHITECTURE, "behavioral", "ander");

	int stmtNum = 1;
	checkConcurrentStatement(tc, prog, unitNum, PROCESS, ++stmtNum, "", NULL, NULL);

	int qstmtNum = 0;
	checkSequentialStatement(tc, prog, unitNum, stmtNum, ++qstmtNum, WHILE_STATEMENT, NULL, NULL, NULL);

	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithIf(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(ander){\n \
			\n \
			sig temp stl;\n \
			\n \
			proc () {\n \
				if ( a ) {\n \
					temp <= '1';\n \
				} elsif ( b ) {\n \
					if ( c ) {\n \
						temp <= '0';\n \
					} elsif ( d ) {\n \
						temp <= '1';\n \
					} else {\n \
						if ( q ) {\n \
							temp <= '1';\n \
						}\n \
						temp <= temp;\n \
					}\n \
					temp <= '0';\n \
				} else {\n \
					temp <= temp;\n \
				}\n \
			}\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);
	checkProgram(tc, prog);

	int unitNum = 0;
	checkDesignUnit(tc, prog, ++unitNum, ARCHITECTURE, "behavioral", "ander");

	int stmtNum = 0;
	checkConcurrentStatement(tc, prog, unitNum, PROCESS, ++stmtNum, "", NULL, NULL);

	int qstmtNum = 0;
	//checkSequentialStatement(tc, prog, unitNum, stmtNum, ++qstmtNum, WHILE_STATEMENT, NULL, NULL, NULL);

	FreeProgram(prog);
	free(input);
}

void TestParse_(CuTest *tc){
	char* input = strdup(" \
		\
	");

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
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithEmptyWhileWait);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithWhileLoop);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithIf);
	SUITE_ADD_TEST(suite, TestParse_);

	return suite;
}

