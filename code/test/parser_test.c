#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cutest.h"
#include "lexer.h"
#include "parser.h"


void TestParser_(CuTest *tc){
	char* input = strdup("");
	InitLexer(input);	

	//test case here
	
	free(input);
}

CuSuite* ParserTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestParser_);

	return suite;
}

