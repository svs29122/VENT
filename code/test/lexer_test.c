#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cutest.h"
#include "lexer.h"

void TestNextToken_SingleToken(CuTest *tc){
	char* input = strdup("+");
	struct lexer* tlex = NewLexer(input); 

	Token nt = NextToken(tlex);

	const	char* expToken = tokenToString(PLUS);
	const char* expLiteral = "+";

	CuAssertStrEquals(tc, expToken, tokenToString(nt.type)); 
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
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
		
		free(nt.literal);	
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
	free(nt.literal);	
	
	expToken = PLUS;
	char* expLiteral2 = "+";
	
	nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral2, nt.literal); 
	
	free(nt.literal);	
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
		
		free(nt.literal);	
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
		
		free(nt.literal);	
		free(expLiteral);
	}

	free(tlex);
	free(input);
}

void TestNextToken_Number (CuTest *tc){
	char* input = strdup("1.6E-20");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = NUMBERLIT;
	char* expLiteral = "1.6E-20";

	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	
	free(nt.literal);	
	free(tlex);
	free(input);
}

void TestNextToken_Char (CuTest *tc){
	char* input = strdup("'1'");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = CHARLIT;
	char* expLiteral = "1";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(tlex);
	free(input);
}

void TestNextToken_BitString (CuTest *tc){
	char* input = strdup("X\"1AFF\"");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = BSTRINGLIT;
	char* expLiteral = "X\"1AFF\"";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(tlex);
	free(input);
}

void TestNextToken_String (CuTest *tc){
	char* input = strdup("\"HELLO\"");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = STRINGLIT;
	char* expLiteral = "\"HELLO\"";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(tlex);
	free(input);
}

void TestNextToken_Identifier (CuTest *tc){
	char* input = strdup("myEntity{");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = IDENTIFIER;
	char* expLiteral = "myEntity";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(tlex);
	free(input);
}

void TestNextToken_IdentifierSingleLetter (CuTest *tc){
	char* input = strdup("a and");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = IDENTIFIER;
	char* expLiteral = "a";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(tlex);
	free(input);
}

void TestNextToken_Entity (CuTest *tc){
	char* input = strdup("ent");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = ENT;
	char* expLiteral = "ent";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(tlex);
	free(input);
}

void TestNextToken_EntityDeclaration (CuTest *tc){
	char* input = strdup("ent ander {\n}");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = ENT;
	char* expLiteral = "ent";
	
	Token nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	free(nt.literal);	

	expToken = IDENTIFIER;
	char* expLiteral2 = "ander";

	nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral2, nt.literal); 
	free(nt.literal);	

	expToken = LBRACE;
	char* expLiteral3 = "{";

	nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral3, nt.literal); 
	free(nt.literal);	

	expToken = RBRACE;
	char* expLiteral4 = "}";

	nt = NextToken(tlex);

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral4, nt.literal); 
	free(nt.literal);	

	free(tlex);
	free(input);
}

void TestNextToken_PortDirections (CuTest *tc){
	char* input = strdup("-> <- <->");
	struct lexer* tlex = NewLexer(input); 

	enum TOKEN_TYPE expToken = INPUT;
	char* expLiteral = "->";
	
	Token nt = NextToken(tlex);

	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	CuAssertIntEquals(tc, expToken, nt.type);
	free(nt.literal);	

	expToken = OUTPUT;
	char* expLiteral2 = "<-";
	
	nt = NextToken(tlex);

	CuAssertStrEquals(tc, expLiteral2, nt.literal); 
	CuAssertIntEquals(tc, expToken, nt.type);
	free(nt.literal);	

	expToken = INOUT;
	char* expLiteral3 = "<->";
	
	nt = NextToken(tlex);

	CuAssertStrEquals(tc, expLiteral3, nt.literal); 
	CuAssertIntEquals(tc, expToken, nt.type);
	free(nt.literal);	

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

