#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lexer.h>
#include <ast.h>

#include "../cutest.h"
#include "../src/parser/parser.c"
#include "../src/parser/error.c"
#include "../src/ast.c"
#include "../src/display.c"

void TestParseInternal_SimpleProcess(CuTest *tc){
   char* input = strdup(" \
      proc (clk) {\n \
				clk <= not clk;\n \
			}\n \
		}\n \
   ");
	
	InitLexer(input);
	initParser();
	
	struct ConcurrentStatement conStmt = {0};
	conStmt.type = PROCESS;

	parseProcessStatement(&(conStmt.as.process));

	// clean up
   free(input);
}

void TestParseInternal_NestedIf(CuTest *tc){
   char* input = strdup(" \
      proc () {\n \
			if(a<b){\n \
				a <= '1';\n \
			} elsif (b<a){\n \
				b <= '1';\n \
				if(c = '1'){\n \
					b <= '0';\n \
				} elsif (c = '0') {\n \
					b <= '1';\n \
				} else {\n \
					b <= '3';\n \
				}\n \
			} else {\n \
				a <= '0';\n \
				b <= '0';\n \
			}\n \
		}\n \
   ");
	
	InitLexer(input);
	initParser();
	
	struct ConcurrentStatement conStmt = {0};
	conStmt.type = PROCESS;

	parseProcessStatement(&(conStmt.as.process));

	struct OperationBlock* op = InitOperationBlock();
	setupDisplayOpBlock(op);

	walkProcessStatement(&(conStmt.as.process), op);

	// clean up
	free(op);
	printf("\e[0m");

   free(input);
}

void TestParseInternal_(CuTest *tc){
   char* input = strdup(" \
      \
   ");

   free(input);
}

CuSuite* ParserInternalsGetSuite(){
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, TestParseInternal_SimpleProcess);
   SUITE_ADD_TEST(suite, TestParseInternal_NestedIf);
   SUITE_ADD_TEST(suite, TestParseInternal_);

   return suite;
}


int main() {

	CuString *output = CuStringNew();
	CuSuite* parserInteralsTestSuite  = ParserInternalsGetSuite();

  // run those babies!
   CuSuiteRun(parserInteralsTestSuite);
   CuSuiteSummary(parserInteralsTestSuite, output);
   CuSuiteDetails(parserInteralsTestSuite, output);

   printf("%s\n", output->buffer);
   CuStringDelete(output);

   // cleanup all test cases and suites
   CuSuiteDelete(parserInteralsTestSuite);

	return 0;
}
