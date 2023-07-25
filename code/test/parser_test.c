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

//helper macros
#define getUseStatement(p, x)		((struct UseStatement*)(ReadBlockArray(p->useStatements, x)))

#define getDesignUnit(p, x) 		((struct DesignUnit*)(ReadBlockArray(p->units, x)))
#define getEntity(d)					(struct EntityDecl*)(&(d->as.entity))
#define getArch(d)					(struct ArchitectureDecl*)(&(d->as.architecture))

#define getPortDecl(d, x)			((struct PortDecl*)(ReadBlockArray((d)->ports, x)))

#define getDeclaration(a, x)		((struct Declaration*)(ReadBlockArray((a)->declarations, x)))
#define getSigDecl(d)				(struct SignalDecl*)(&(d->as.signalDeclaration))
#define getVarDecl(d)				(struct VariableDecl*)(&(d->as.variableDeclaration))

#define getConStatement(a, x)		((struct ConcurrentStatement*)(ReadBlockArray((a)->statements, x)))
#define getProcess(c)				(struct Process*)(&(c->as.process))

#define getSeqStatement(p, x)		((struct SequentialStatement*)(ReadBlockArray((p)->statements, x)))
#define getSigAssign(s)				(struct SignalAssign*)(&(s->as.signalAssignment))
#define getVarAssign(s)				(struct VariableAssign*)(&(s->as.variableAssignment))
#define getWaitStatement(s)		(struct WaitStatement*)(&(s->as.waitStatement))
#define getWhileStatement(s)		(struct WhileStatement*)(&(s->as.whileStatement))

#define getIfStatement(s)			(struct IfStatement*)(&(s->as.ifStatement))
#define getConsequent(p, x)		((struct SequentialStatement*)(ReadBlockArray((p)->consequentStatements, x)))
#define getAlternative(p, x)		((struct SequentialStatement*)(ReadBlockArray((p)->alternativeStatements, x)))

#define getBinaryExp(e)				(struct BinaryExpr*)(e)
#define getNumExp(e)					(struct NumExpr*)(e)
#define getIdentifier(e)			(struct Identifier*)(e)

// the actual tests
void TestParseProgram_UseDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all;");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", (getUseStatement(prog, 0))->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_EntityDeclarationNoPorts(CuTest *tc){
	char* input = strdup("ent ander {\n}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	
	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", (getEntity(getDesignUnit(prog, 0)))->name->value);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_UseWithEntityDeclaration(CuTest *tc){
	char* input = strdup("use ieee.std_logic_1164.all;\n\nent ander {\n}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", (getUseStatement(prog, 0))->value);

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", (getEntity(getDesignUnit(prog, 0)))->name->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_EntityDeclarationWithPorts(CuTest *tc){
	char* input = strdup("ent ander{ a -> stl; b -> stl; y <- stl;}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", (getEntity(getDesignUnit(prog, 0)))->name->value);

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
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", (getUseStatement(prog, 0))->value);

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", (getEntity(getDesignUnit(prog, 0)))->name->value);

	struct PortDecl* port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 0);	
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "a", port->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

	port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 1);	
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "b", port->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

	port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 2);	
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "y", port->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "<-", port->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureDeclarationEmpty(CuTest *tc){
	char* input = strdup("arch behavioral(ander) {}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);
	
	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalDeclaration(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl;}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);
	
	CuAssertStrEquals(tc, "temp", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->name->value);
	CuAssertStrEquals(tc, "stl", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->dtype->value);

	FreeProgram(prog);	
	free(input);
}

void TestParseProgram_ArchitectureWithSignalInit(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl := '0';}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);

	CuAssertStrEquals(tc, "temp", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->name->value);
	CuAssertStrEquals(tc, "stl", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->dtype->value);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssign(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= '0';}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);

	CuAssertStrEquals(tc, "temp", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->name->value);
	CuAssertStrEquals(tc, "stl", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->dtype->value);

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", (getSigAssign(getConStatement(getArch(getDesignUnit(prog, 0)), 0)))->target->value);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ArchitectureWithSignalAssignBinaryExpression(CuTest *tc){
	char* input = strdup("arch behavioral(ander) { sig temp stl; temp <= a and b;}");

	struct Program* prog = ParseProgram(input);
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);

	CuAssertStrEquals(tc, "temp", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->name->value);
	CuAssertStrEquals(tc, "stl", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->dtype->value);

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", (getSigAssign(getConStatement(getArch(getDesignUnit(prog, 0)), 0)))->target->value);

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
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
	CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
	CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", (getUseStatement(prog, 0))->value);

	int unitNum = 1;
	int portNum = 1;

	CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
	CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", (getEntity(getDesignUnit(prog, 0)))->name->value);

	struct PortDecl* port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 0);	
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "a", port->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

	port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 1);	
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "b", port->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

	port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 2);	
	CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "y", port->name->value);
	CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "<-", port->pmode->value);
	CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

	unitNum += 1;
	int declNum = 1;
	int stmtNum = 1;

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 1)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 1)))->entName->value);

	CuAssertStrEquals(tc, "temp", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 1)), 0)))->name->value);
	CuAssertStrEquals(tc, "stl", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 1)), 0)))->dtype->value);

	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", (getSigAssign(getConStatement(getArch(getDesignUnit(prog, 1)), 0)))->target->value);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "y", (getSigAssign(getConStatement(getArch(getDesignUnit(prog, 1)), 1)))->target->value);

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
		
		struct Program* prog = ParseProgram(input);
		CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	
		CuAssertPtrNotNullMsg(tc,"Use statements NULL!", prog->useStatements);	
		CuAssertStrEquals_Msg(tc,"use path incorrect!", "ieee.std_logic_1164.all", (getUseStatement(prog, 0))->value);

		CuAssertPtrNotNullMsg(tc,"Design units NULL!", prog->units);	
		CuAssertStrEquals_Msg(tc,"Entity identifier incorrect!", "ander", (getEntity(getDesignUnit(prog, 0)))->name->value);

		struct PortDecl* port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 0);	
		CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "a", port->name->value);
		CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port->pmode->value);
		CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

		port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 1);	
		CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "b", port->name->value);
		CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "->", port->pmode->value);
		CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);
	
		port = getPortDecl(getEntity(getDesignUnit(prog, 0)), 2);	
		CuAssertStrEquals_Msg(tc,"Port identifier incorrect!", "y", port->name->value);
		CuAssertStrEquals_Msg(tc,"Port mode incorrect!", "<-", port->pmode->value);
		CuAssertStrEquals_Msg(tc,"Port data type incorrect!", "stl", port->dtype->value);

		CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 1)))->archName->value);
		CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 1)))->entName->value);

		CuAssertStrEquals(tc, "temp", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 1)), 0)))->name->value);
		CuAssertStrEquals(tc, "stl", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 1)), 0)))->dtype->value);

		CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "temp", (getSigAssign(getConStatement(getArch(getDesignUnit(prog, 1)), 0)))->target->value);
		CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "y", (getSigAssign(getConStatement(getArch(getDesignUnit(prog, 1)), 1)))->target->value);

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
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	int unitNum = 1;
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);

	CuAssertStrEquals(tc, "a", 	(getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->name->value);
	CuAssertStrEquals(tc, "stl", 	(getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->dtype->value);
	CuAssertStrEquals(tc, "b", 	(getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 1)))->name->value);
	CuAssertStrEquals(tc, "stl", 	(getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 1)))->dtype->value);
	CuAssertStrEquals(tc, "y", 	(getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 2)))->name->value);
	CuAssertStrEquals(tc, "stl", 	(getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 2)))->dtype->value);
	
	struct Process* proc = getProcess(getConStatement(getArch(getDesignUnit(prog, 0)), 0));
	CuAssertStrEquals(tc, "clk", proc->sensitivityList->value);
	CuAssertStrEquals_Msg(tc,"Signal identifier incorrect!", "y", (getSigAssign(getSeqStatement(proc, 0)))->target->value);

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
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	int unitNum = 1;
	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);

	CuAssertStrEquals(tc, "u", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->name->value);
	CuAssertStrEquals(tc, "stl", (getSigDecl(getDeclaration(getArch(getDesignUnit(prog, 0)), 0)))->dtype->value);
	
	struct Process* proc = getProcess(getConStatement(getArch(getDesignUnit(prog, 0)), 0));
	CuAssertStrEquals(tc, "s", (getSigDecl(getDeclaration(proc, 0)))->name->value);
	CuAssertStrEquals(tc, "i", (getVarDecl(getDeclaration(proc, 1)))->name->value);

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
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 0)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 0)))->entName->value);

	struct Process* proc = getProcess(getConStatement(getArch(getDesignUnit(prog, 0)), 0));
	CuAssertIntEquals_Msg(tc, "Sequential statement type incorrect!", WHILE_STATEMENT, (getSeqStatement(proc, 0))->type);	
	CuAssertIntEquals_Msg(tc, "Sequential statement type incorrect!", WAIT_STATEMENT, (getSeqStatement(proc, 1))->type);	

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
	CuAssertPtrNotNullMsg(tc,"ParseProgram() returned NULL!", prog);	

	CuAssertStrEquals_Msg(tc,"Architecture identifier incorrect!", "behavioral", (getArch(getDesignUnit(prog, 1)))->archName->value);
	CuAssertStrEquals_Msg(tc,"Architecture entity binding incorrect!", "ander", (getArch(getDesignUnit(prog, 1)))->entName->value);

	struct Process* proc = getProcess(getConStatement(getArch(getDesignUnit(prog, 1)), 1));
	CuAssertStrEquals(tc, "s", (getSigDecl(getDeclaration(proc, 0)))->name->value);
	CuAssertStrEquals(tc, "i", (getVarDecl(getDeclaration(proc, 1)))->name->value);

	CuAssertIntEquals_Msg(tc, "Sequential statement type incorrect!", WHILE_STATEMENT, (getSeqStatement(proc, 0))->type);	
	CuAssertIntEquals_Msg(tc, "Sequential statement type incorrect!", WAIT_STATEMENT, (getSeqStatement(proc, 1))->type);	

	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithIf(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(ander){\n \
			\n \
			proc (clk) {\n \
				var myVar stl;\n \
				if (a>5) {\n \
				} elsif (b) {\n \
					if ( c == 255 ) {\n \
						temp <= '0';\n \
					} elsif (d=='0') {\n \
						temp <= '1';\n \
					} else {\n \
						if ( q == y ) {\n \
							temp <= '1';\n \
						}\n \
						myVar := '1';\n \
					}\n  \
					temp <= '0';\n \
				} else {\n \
					temp <= temp;\n \
				}\n \
				wait;\n \
			}\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);

	struct Process* proc = getProcess(getConStatement(getArch(getDesignUnit(prog, 0)), 0));
	CuAssertStrEquals(tc, "clk", proc->sensitivityList->value);
	CuAssertStrEquals(tc, "myVar", (getVarDecl(getDeclaration(proc, 0)))->name->value);

	struct IfStatement* ifs = getIfStatement(getSeqStatement(proc, 0));
	struct BinaryExpr* be = getBinaryExp(ifs->antecedent);
	CuAssertStrEquals(tc, "a", (getIdentifier(be->left))->value);
	CuAssertStrEquals(tc, ">", be->op);
	CuAssertStrEquals(tc, "5", (getNumExp(be->right))->literal);

	CuAssertStrEquals(tc, "temp", (getSigAssign(getAlternative(ifs, 0)))->target->value);

	CuAssertIntEquals_Msg(tc, "Sequential statement type incorrect!", WAIT_STATEMENT, (getSeqStatement(proc, 1))->type);	
	
	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithNestedIf(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(ander){\n \
			\n \
			proc () {\n \
				if(a<b){\n \
					a <= '1';\n \
				} elsif (b<a){\n \
					b <= '1';\n \
					if(c == '1'){\n \
						b <= '0';\n \
					} elsif (c == '0') {\n \
						b <= '1';\n \
					} else {\n \
						b <= '3';\n \
					}\n \
				} else {\n \
					a <= '0';\n \
					b <= '0';\n \
				}\n \
			}\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);

	CuAssertTrue(tc, ThereWasAnError() == false);
	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithInfiniteLoop(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(looper){\n \
			\n \
			proc() {\n \
				loop {\n \
					count := count + 1;\n \
					count += 1;\n \
					count++;\n \
				}\n \
			}\n \
		}\n \
		\
	");

	struct Program* prog = ParseProgram(input);

	CuAssertTrue(tc, ThereWasAnError() == false);
	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithForLoop(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(looper){\n \
			\n \
			proc() {\n \
				for (i : 0 to 5){\n \
					count := count + 1;\n \
				}\n \
				\n \
				for (op : Opcode) {\n \
					muxIn <= op;\n \
				}\n \
			}\n \
		}\n \
		\
	");

	struct Program* prog = ParseProgram(input);

	CuAssertTrue(tc, ThereWasAnError() == false);
	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithSwitchCase(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(switcher){\n \
			\n \
			proc() {\n \
				switch(ADDRESS) {\n \
					case 0:\n \
						A <= '1';\n \
					case 1:\n \
						A <= '1';\n \
						B <= '1';\n \
					case 2 to 15:\n \
						C <= '1';\n \
					case 16 | 10 downto 20 | 25:\n \
						B <= '1';\n \
						C <= '1';\n \
						D := '1';\n \
					default:\n \
						null;\n \
				}\n \
			}\n \
		}\n \
		\
	");

	struct Program* prog = ParseProgram(input);

	CuAssertTrue(tc, ThereWasAnError() == false);
	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithAssert(CuTest *tc){
	char* input = strdup(" \
		arch tb(uut){\n \
			\n \
			proc(clk) {\n \
				var test int := 1;\n \
				while(test != 10){\n \
					test++;\n \
					report \"bumping test\" severity note;\n \
					\n \
				}\n \
				assert(test != 11) ;\n \
				assert(test != 11) report \"Test out of bounds!\" severity error;\n \
				report \"Test Passed!!!\";\n \
			}\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);

	CuAssertTrue(tc, ThereWasAnError() == false);
	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParseProgram_ProcessWithTypeDefinition(CuTest *tc){
	char* input = strdup(" \
		arch behavioral(looper){\n \
			\n \
			sig temp stl;\n \
			type OpCode {Idle, Start, Stop, Clear};\n \
			type myLogic {'0', '1', '2', 'F'};\n \
			\n \
			proc() {\n \
				for (op : Opcode) {\n \
					temp <= op;\n \
				}\n \
			}\n \
		}\n \
		\
	");

	struct Program* prog = ParseProgram(input);

	CuAssertTrue(tc, ThereWasAnError() == false);
	//PrintProgram(prog);

	FreeProgram(prog);
	free(input);
}

void TestParse_EntityWithGeneric(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all;\n \
		ent ander {\n \
			g int;\n \
			a -> stl;\n \
			b -> stl;\n \
			y <- stl; \n \
		}\n \
		\
	");

	struct Program* prog = ParseProgram(input);

	CuAssertTrue(tc, ThereWasAnError() == false);
	PrintProgram(prog);

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
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithNestedIf);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithInfiniteLoop);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithForLoop);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithSwitchCase);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithAssert);
	SUITE_ADD_TEST(suite, TestParseProgram_ProcessWithTypeDefinition);
	SUITE_ADD_TEST(suite, TestParse_EntityWithGeneric);
	SUITE_ADD_TEST(suite, TestParse_);

	return suite;
}

