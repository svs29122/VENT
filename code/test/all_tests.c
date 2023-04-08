#include <stdlib.h>
#include <stdio.h>

#include "cutest.h"

// convenience macros
//#define TEST_LEXER
//#define TEST_PARSER
//#define TEST_TRANSPILE

CuSuite* LexerTestGetSuite();
CuSuite* ParserTestGetSuite();
CuSuite* TranspileTestGetSuite();

void RunAllTests(void){
	CuString *output = CuStringNew();
	CuSuite* masterSuite = CuSuiteNew();

	// add new suites here!
#ifdef TEST_LEXER
	CuSuite* lexerTestSuite = LexerTestGetSuite();
	CuSuiteAddSuite(masterSuite, lexerTestSuite);
#endif
#ifdef TEST_PARSER
	CuSuite* parserTestSuite = ParserTestGetSuite();
	CuSuiteAddSuite(masterSuite, parserTestSuite);
#endif
	CuSuite* transpileTestSuite = TranspileTestGetSuite();
	CuSuiteAddSuite(masterSuite, transpileTestSuite);

	// run those babies!
	CuSuiteRun(masterSuite);
	CuSuiteSummary(masterSuite, output);
	CuSuiteDetails(masterSuite, output);

	printf("%s\n", output->buffer);
	CuStringDelete(output);

	// cleanup all test cases and suites
	CuSuiteDelete(transpileTestSuite);
#ifdef TEST_PARSER
	CuSuiteDelete(parserTestSuite);
#endif
#ifdef TEST_LEXER
	CuSuiteDelete(lexerTestSuite);
#endif
	free(masterSuite);
}

int main(void) {
	RunAllTests();
	return 0;
}
