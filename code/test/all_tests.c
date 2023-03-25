#include <stdlib.h>
#include <stdio.h>

#include "cutest.h"

CuSuite* LexerTestGetSuite();
CuSuite* ParserTestGetSuite();

void RunAllTests(void){
	CuString *output = CuStringNew();
	CuSuite* masterSuite = CuSuiteNew();

	// add new suites here!
	CuSuite* lexerTestSuite = LexerTestGetSuite();
	CuSuiteAddSuite(masterSuite, lexerTestSuite);

	CuSuite* parserTestSuite = ParserTestGetSuite();
	CuSuiteAddSuite(masterSuite, parserTestSuite);

	// run those babies!
	CuSuiteRun(masterSuite);
	CuSuiteSummary(masterSuite, output);
	CuSuiteDetails(masterSuite, output);

	printf("%s\n", output->buffer);
	CuStringDelete(output);

	// cleanup all test cases and suites
	CuSuiteDelete(parserTestSuite);
	CuSuiteDelete(lexerTestSuite);
	free(masterSuite);
}

int main(void) {
	RunAllTests();
	return 0;
}
