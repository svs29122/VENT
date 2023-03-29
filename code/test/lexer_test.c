#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cutest.h"
#include "lexer.h"

void TestNextToken_SingleToken(CuTest *tc){
	char* input = strdup("+");
	InitLexer(input);

	Token nt = NextToken();

	const	char* expToken = TokenToString(TOKEN_PLUS);
	const char* expLiteral = "+";

	CuAssertStrEquals(tc, expToken, TokenToString(nt.type)); 
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
}

void TestNextToken_MultipleTokens(CuTest *tc){
	char* input = strdup("():{},'/-+*=");
	InitLexer(input);

	enum TOKEN_TYPE expToken[12] = {TOKEN_LPAREN,TOKEN_RPAREN,TOKEN_COLON, 
											TOKEN_LBRACE,TOKEN_RBRACE,TOKEN_COMMA,
											TOKEN_TICK,TOKEN_SLASH,TOKEN_MINUS,
											TOKEN_PLUS,TOKEN_STAR,TOKEN_EQUAL};
	
	char expLiteralArr[12] = {'(', ')', ':', 
								 '{', '}',',',
								 '\'', '/', '-',
								  '+', '*','='};

	for(int i=0; i<12; i++){
		Token nt = NextToken();
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(nt.literal);	
		free(expLiteral);
	}

	free(input);
}

void TestNextToken_SingleLineComment (CuTest *tc){
	char* input = strdup("*" 
								"//comment\n"
								"+");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_STAR;
	char* expLiteral = "*";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	free(nt.literal);	
	
	expToken = TOKEN_PLUS;
	char* expLiteral2 = "+";
	
	nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral2, nt.literal); 
	
	free(nt.literal);	
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
	InitLexer(input);

	enum TOKEN_TYPE expToken[6] = {TOKEN_LPAREN,TOKEN_PLUS,TOKEN_RPAREN,TOKEN_LBRACE,TOKEN_STAR,TOKEN_RBRACE};
	char expLiteralArr[6] = {'(','+', ')', '{', '*', '}'}; 

	for(int i=0; i<6; i++){
		Token nt = NextToken();
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(nt.literal);	
		free(expLiteral);
	}

	free(input);
}

void TestNextToken_MultiLineCommentUnterminated (CuTest *tc){
	char* input = strdup("+/**+");
	InitLexer(input);

	enum TOKEN_TYPE expToken[2] = {TOKEN_PLUS,TOKEN_EOP};
	char expLiteralArr[2] = {'+','\0'}; 
	
	
	for(int i=0; i<2; i++){
		Token nt = NextToken();
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(nt.literal);	
		free(expLiteral);
	}

	free(input);
}

void TestNextToken_Number (CuTest *tc){
	char* input = strdup("1.6E-20");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_NUMBERLIT;
	char* expLiteral = "1.6E-20";

	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	
	free(nt.literal);	
	free(input);
}

void TestNextToken_Char (CuTest *tc){
	char* input = strdup("'1'");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_CHARLIT;
	char* expLiteral = "1";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
}

void TestNextToken_BitString (CuTest *tc){
	char* input = strdup("X\"1AFF\"");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_BSTRINGLIT;
	char* expLiteral = "X\"1AFF\"";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
}

void TestNextToken_String (CuTest *tc){
	char* input = strdup("\"HELLO\"");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_STRINGLIT;
	char* expLiteral = "\"HELLO\"";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
}

void TestNextToken_Identifier (CuTest *tc){
	char* input = strdup("myEntity{");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_IDENTIFIER;
	char* expLiteral = "myEntity";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
}

void TestNextToken_IdentifierSingleLetter (CuTest *tc){
	char* input = strdup("a and");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_IDENTIFIER;
	char* expLiteral = "a";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
}

void TestNextToken_Entity (CuTest *tc){
	char* input = strdup("ent");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_ENT;
	char* expLiteral = "ent";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
}

void TestNextToken_EntityDeclaration (CuTest *tc){
	char* input = strdup("ent ander {\n}");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_ENT;
	char* expLiteral = "ent";
	
	Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	free(nt.literal);	

	expToken = TOKEN_IDENTIFIER;
	char* expLiteral2 = "ander";

	nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral2, nt.literal); 
	free(nt.literal);	

	expToken = TOKEN_LBRACE;
	char* expLiteral3 = "{";

	nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral3, nt.literal); 
	free(nt.literal);	

	expToken = TOKEN_RBRACE;
	char* expLiteral4 = "}";

	nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral4, nt.literal); 
	free(nt.literal);	

	free(input);
}

void TestNextToken_PortDirections (CuTest *tc){
	char* input = strdup("-> <- <->");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_INPUT;
	char* expLiteral = "->";
	
	Token nt = NextToken();

	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	CuAssertIntEquals(tc, expToken, nt.type);
	free(nt.literal);	

	expToken = TOKEN_OUTPUT;
	char* expLiteral2 = "<-";
	
	nt = NextToken();

	CuAssertStrEquals(tc, expLiteral2, nt.literal); 
	CuAssertIntEquals(tc, expToken, nt.type);
	free(nt.literal);	

	expToken = TOKEN_INOUT;
	char* expLiteral3 = "<->";
	
	nt = NextToken();

	CuAssertStrEquals(tc, expLiteral3, nt.literal); 
	CuAssertIntEquals(tc, expToken, nt.type);
	free(nt.literal);	

	free(input);
}

void TestNextToken_ (CuTest *tc){
	char* input = strdup("");
	InitLexer(input);

	//test case here
	
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
	SUITE_ADD_TEST(suite, TestNextToken_Char);
	SUITE_ADD_TEST(suite, TestNextToken_BitString);
	SUITE_ADD_TEST(suite, TestNextToken_String);
	SUITE_ADD_TEST(suite, TestNextToken_Identifier);
	SUITE_ADD_TEST(suite, TestNextToken_IdentifierSingleLetter);
	SUITE_ADD_TEST(suite, TestNextToken_Entity);
	SUITE_ADD_TEST(suite, TestNextToken_EntityDeclaration);
	SUITE_ADD_TEST(suite, TestNextToken_PortDirections);

	return suite;
}

