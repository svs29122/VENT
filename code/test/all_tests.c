#include <stdio.h>

#include "cutest.h"

CuSuite* LexerTestGetSuite();
CuSuite* ParserTestGetSuite();

void RunAllTests(void){
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	// add new suites here!
	//CuSuiteAddSuite(suite, LexerTestGetSuite());
	CuSuiteAddSuite(suite, ParserTestGetSuite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

int main(void) {
	RunAllTests();
	return 0;
}
