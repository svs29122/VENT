#include <stdlib.h>
#include <stdio.h>

#include "cutest.h"

// convenience macros
#define TEST_DBA
#define TEST_DHT
#define TEST_LEXER
#define TEST_PARSER
#define TEST_TRANSPILE

CuSuite* DbaTestGetSuite();
CuSuite* DhtTestGetSuite();
CuSuite* LexerTestGetSuite();
CuSuite* ParserTestGetSuite();
CuSuite* TranspileTestGetSuite();

void RunAllTests(void){
	printf("*** Running VENT unit tests ***\r\n");
	CuString *output = CuStringNew();
	CuSuite* masterSuite = CuSuiteNew();

	// add new suites here!
#ifdef TEST_DBA
	CuSuite* dbaTestSuite = DbaTestGetSuite();
	CuSuiteAddSuite(masterSuite, dbaTestSuite);
#endif
#ifdef TEST_DHT
	CuSuite* dhtTestSuite = DhtTestGetSuite();
	CuSuiteAddSuite(masterSuite, dhtTestSuite);
#endif
#ifdef TEST_LEXER
	CuSuite* lexerTestSuite = LexerTestGetSuite();
	CuSuiteAddSuite(masterSuite, lexerTestSuite);
#endif
#ifdef TEST_PARSER
	CuSuite* parserTestSuite = ParserTestGetSuite();
	CuSuiteAddSuite(masterSuite, parserTestSuite);
#endif
#ifdef TEST_TRANSPILE
	CuSuite* transpileTestSuite = TranspileTestGetSuite();
	CuSuiteAddSuite(masterSuite, transpileTestSuite);
#endif

	// run those babies!
	CuSuiteRun(masterSuite);
	CuSuiteSummary(masterSuite, output);
	CuSuiteDetails(masterSuite, output);

	printf("%s\n", output->buffer);
	CuStringDelete(output);

	// cleanup all test cases and suites
#ifdef TEST_TRANSPILE
	CuSuiteDelete(transpileTestSuite);
#endif
#ifdef TEST_PARSER
	CuSuiteDelete(parserTestSuite);
#endif
#ifdef TEST_LEXER
	CuSuiteDelete(lexerTestSuite);
#endif
#ifdef TEST_DBA
	CuSuiteDelete(dbaTestSuite);
#endif
#ifdef TEST_DHT
	CuSuiteDelete(dhtTestSuite);
#endif
	free(masterSuite);
}

int main(void) {
	RunAllTests();
	return 0;
}
