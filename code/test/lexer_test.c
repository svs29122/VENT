#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "cutest.h"
#include "lexer.h"

void TestNextToken_SingleToken(CuTest *tc){
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

void TestNextToken_MultipleTokens(CuTest *tc){
	char* input = strdup("():{},'/-+*=");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken[12] = {LPAREN,RPAREN,COLON, 
											LBRACE,RBRACE,COMMA,
											TICK,SLASH,MINUS,
											PLUS,STAR,EQUAL};
	
	char expLiteralArr[12] = {'(', ')', ':', 
								 '{', '}',',',
								 '\'', '/', '-',
								  '+', '*','='};

	for(int i=0; i<12; i++){
		Token nt = NextToken(tlex);
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(expLiteral);
	}

	free(tlex);
	free(input);
}

void TestNextToken_SingleLineComment (CuTest *tc){
	char* input = strdup("*" 
								"//comment\n"
								"+");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = STAR;
	char* expLiteral = "*";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	
	expToken = PLUS;
	char* expLiteral2 = "+";
	
	nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral2, nt.literal); 
	
	free(tlex);
	free(input);
}

void TestNextToken_MultiLineComment (CuTest *tc){
	char* input = strdup(" \
(+) \
/* This is a \
multiline comment \
that explains ** something \
important*/ \
{*}");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken[6] = {LPAREN,PLUS,RPAREN,LBRACE,STAR,RBRACE};
	char expLiteralArr[6] = {'(','+', ')', '{', '*', '}'}; 

	for(int i=0; i<6; i++){
		Token nt = NextToken(tlex);
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(expLiteral);
	}

	free(tlex);
	free(input);
}

void TestNextToken_MultiLineCommentUnterminated (CuTest *tc){
	char* input = strdup("+/**+");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken[2] = {PLUS,ILLEGAL};
	char expLiteralArr[2] = {'+','\0'}; 
	
	
	for(int i=0; i<2; i++){
		Token nt = NextToken(tlex);
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(expLiteral);
	}

	free(tlex);
	free(input);
}

void TestNextToken_Number (CuTest *tc){
	char* input = strdup("5");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = NUMBER;
	char* expLiteral = "5";

	Token nt = NextToken(tlex);
	
	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	
	free(tlex);
	free(input);
}

void TestNextToken_ (CuTest *tc){
	char* input = strdup("");
	struct lexer* tlex = NewLexer(input); 

	//test case here
	
	free(tlex);
	free(input);
}

CuSuite* LexerTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestNextToken_SingleToken);
	SUITE_ADD_TEST(suite, TestNextToken_MultipleTokens);
	SUITE_ADD_TEST(suite, TestNextToken_SingleLineComment);
	SUITE_ADD_TEST(suite, TestNextToken_MultiLineComment);
	SUITE_ADD_TEST(suite, TestNextToken_MultiLineCommentUnterminated);
	SUITE_ADD_TEST(suite, TestNextToken_Number);

	return suite;
}

