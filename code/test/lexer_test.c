#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cutest.h"
#include "lexer.h"

void TestNextToken_Single(CuTest *tc){
	char* input = strdup("+");
	struct lexer* tlex = NewLexer(input); 

	Token nt = NextToken(tlex);

	enum TOKEN_TYPE expToken = PLUS;
	char* expLiteral = "+";

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	
	free(tlex);
	free(input);
}

void TestNextToken_Multiple(CuTest *tc){
	char* input = strdup("():{},'/-+*=");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken[12] = {LPAREN,RPAREN,COLON, 
											LBRACE,RBRACE,COMMA,
											TICK,SLASH,MINUS,
											PLUS,STAR,EQUAL};
	
	char expLiteral[12] = {'(', ')', ':', 
								 '{', '}',',',
								 '\'', '/', '-',
								  '+', '*','='};

	for(int i=0; i<12; i++){
		Token nt = NextToken(tlex);
	
		char* eliteral = (char*) malloc(sizeof(char) * 2);
		eliteral[0] = expLiteral[i];
		eliteral[1] = '\0';

		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, eliteral, nt.literal); 
		
		free(eliteral);
	}

	free(tlex);
}

CuSuite* LexerTestGetSuite(){
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, TestNextToken_Single);
	SUITE_ADD_TEST(suite, TestNextToken_Multiple);
	return suite;
}

