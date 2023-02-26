#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cutest.h"
#include "lexer.h"

void TestNextToken(CuTest *tc){
	char* input = strdup("+");
	struct lexer* tlex = NewLexer(input); 

	Token nt = NextToken(tlex);

	enum TOKEN_TYPE expToken = PLUS;
	char* expLiteral = "+";

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	
	free(tlex);
}

CuSuite* LexerTestGetSuite(){
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestNextToken);
	return suite;
}

