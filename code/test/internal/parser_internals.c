#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lexer.h>
#include <ast.h>

#include "../cutest.h"
#include "../src/parser/parser.c"

void TestParseInternal_SimpleProcess(CuTest *tc){
   char* input = strdup(" \
      proc ( clk ) {\n \
			clk <=  not clk;\n \
		}\n \
   ");
	
	InitLexer(input);
	initParser();
	
	struct ConcurrentStatement conStmt = {0};
	conStmt.type = PROCESS;

	parseProcessStatement(&(conStmt.as.process));

	
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
