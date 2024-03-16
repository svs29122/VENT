#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <lexer.h>

#include "cutest.h"

static void testNextToken(CuTest* tc,  enum TOKEN_TYPE type, char* lit){

	struct Token tk = NextToken();
	if(lit != NULL)
		CuAssertStrEquals(tc, lit, tk.literal); 
	CuAssertStrEquals(tc, TokenToString(type), TokenToString(tk.type));
	free(tk.literal);	
}

void TestNextToken_SingleToken(CuTest *tc){
	char* input = strdup("+");
	InitLexer(input);

	struct Token nt = NextToken();

	const	char* expToken = TokenToString(TOKEN_PLUS);
	const char* expLiteral = "+";

	CuAssertStrEquals(tc, expToken, TokenToString(nt.type)); 
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_MultipleTokens(CuTest *tc){
	char* input = strdup("():{},'/-+*");
	InitLexer(input);

	enum TOKEN_TYPE expToken[11] = {TOKEN_LPAREN,TOKEN_RPAREN,TOKEN_COLON, 
											TOKEN_LBRACE, TOKEN_RBRACE,TOKEN_COMMA,
											TOKEN_TICK, TOKEN_SLASH,
											TOKEN_MINUS, TOKEN_PLUS,TOKEN_STAR};
	
	char expLiteralArr[11] = {'(', ')', ':', 
								 '{', '}',',',
								 '\'', '/',
								  '-','+', '*'};

	for(int i=0; i<11; i++){
		struct Token nt = NextToken();
		CuAssertStrEquals(tc, TokenToString(expToken[i]), TokenToString(nt.type)); 
		free(nt.literal);	
	}

	free(input);
	FreeLexer();
}

void TestNextToken_SingleLineComment (CuTest *tc){
	char* input = strdup("*" 
								"//comment\n"
								"+");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_STAR;
	char* expLiteral = "*";
	
	struct Token nt = NextToken();

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
	FreeLexer();
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
		struct Token nt = NextToken();
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(nt.literal);	
		free(expLiteral);
	}

	free(input);
	FreeLexer();
}

void TestNextToken_MultiLineCommentUnterminated (CuTest *tc){
	char* input = strdup("+/**+");
	InitLexer(input);

	enum TOKEN_TYPE expToken[2] = {TOKEN_PLUS,TOKEN_EOP};
	char expLiteralArr[2] = {'+','\0'}; 
	
	
	for(int i=0; i<2; i++){
		struct Token nt = NextToken();
	
		char* expLiteral = strdup((char[2]){expLiteralArr[i], '\0'});
		CuAssertIntEquals(tc, expToken[i], nt.type);
		CuAssertStrEquals(tc, expLiteral, nt.literal); 
		
		free(nt.literal);	
		free(expLiteral);
	}

	free(input);
	FreeLexer();
}

void TestNextToken_Number (CuTest *tc){
	char* input = strdup("1.6E-20");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_NUMBERLIT;
	char* expLiteral = "1.6E-20";

	struct Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 
	
	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_Char (CuTest *tc){
	char* input = strdup("'1'");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_CHARLIT;
	char* expLiteral = "1";
	
	struct Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_BitString (CuTest *tc){
	char* input = strdup("X\"0000\"");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_BSTRINGLIT;
	char* expLiteral = "X\"0000\"";
	
	struct Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_String (CuTest *tc){
	char* input = strdup("\"HELLO\"");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_STRINGLIT;
	char* expLiteral = "\"HELLO\"";
	
	struct Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_Identifier (CuTest *tc){
	char* input = strdup("myEntity{");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_IDENTIFIER;
	char* expLiteral = "myEntity";
	
	struct Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_IdentifierSingleLetter (CuTest *tc){
	char* input = strdup("a and");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_IDENTIFIER;
	char* expLiteral = "a";
	
	struct Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_Entity (CuTest *tc){
	char* input = strdup("ent");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_ENT;
	char* expLiteral = "ent";
	
	struct Token nt = NextToken();

	CuAssertIntEquals(tc, expToken, nt.type);
	CuAssertStrEquals(tc, expLiteral, nt.literal); 

	free(nt.literal);	
	free(input);
	FreeLexer();
}

void TestNextToken_EntityDeclaration (CuTest *tc){
	char* input = strdup("ent ander {\n}");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_ENT;
	char* expLiteral = "ent";
	
	struct Token nt = NextToken();

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
	FreeLexer();
}

void TestNextToken_PortDirections (CuTest *tc){
	char* input = strdup("-> <- <->");
	InitLexer(input);

	enum TOKEN_TYPE expToken = TOKEN_INPUT;
	char* expLiteral = "->";
	
	struct Token nt = NextToken();

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
	FreeLexer();
}

void TestNextToken_ProcessDeclaration (CuTest *tc){
	char* input = strdup("proc (clk) { }");
	InitLexer(input);

	struct Token nt = NextToken();
	CuAssertStrEquals(tc, "proc", nt.literal); 
	CuAssertStrEquals(tc, TokenToString(TOKEN_PROC), TokenToString(nt.type));
	free(nt.literal);	

	nt = NextToken();
	CuAssertIntEquals(tc, TOKEN_LPAREN, nt.type);
	free(nt.literal);	

	nt = NextToken();
	CuAssertStrEquals(tc, "clk", nt.literal); 
	CuAssertStrEquals(tc, TokenToString(TOKEN_IDENTIFIER), TokenToString(nt.type));
	free(nt.literal);	

	nt = NextToken();
	CuAssertIntEquals(tc, TOKEN_RPAREN, nt.type);
	free(nt.literal);	

	nt = NextToken();
	CuAssertIntEquals(tc, TOKEN_LBRACE, nt.type);
	free(nt.literal);	

	nt = NextToken();
	CuAssertIntEquals(tc, TOKEN_RBRACE, nt.type);
	free(nt.literal);	

	free(input);
	FreeLexer();
}

void TestNextToken_Type (CuTest *tc){
	char* input = strdup("type state { start, stop, pause, continue, end };");
	InitLexer(input);

	testNextToken(tc, TOKEN_TYPE, "type");	
	testNextToken(tc, TOKEN_IDENTIFIER, "state");
	testNextToken(tc, TOKEN_LBRACE, "{");	
	testNextToken(tc, TOKEN_IDENTIFIER, "start");
	testNextToken(tc, TOKEN_COMMA, ",");
	testNextToken(tc, TOKEN_IDENTIFIER, "stop");
	testNextToken(tc, TOKEN_COMMA, ",");
	testNextToken(tc, TOKEN_IDENTIFIER, "pause");
	testNextToken(tc, TOKEN_COMMA, ",");
	testNextToken(tc, TOKEN_IDENTIFIER, "continue");
	testNextToken(tc, TOKEN_COMMA, ",");
	testNextToken(tc, TOKEN_IDENTIFIER, "end");
	testNextToken(tc, TOKEN_RBRACE, "}");	
	
	free(input);
	FreeLexer();
}

void TestNextToken_Component (CuTest *tc){
	char* input = strdup("\
	comp counter {\
		clk -> stl; \
		rst -> stl; \
		upDown -> stl;\
		Q <- stlv(3 downto 0);\
	}\
	");

	InitLexer(input);

	testNextToken(tc, TOKEN_COMP, "comp");	
	testNextToken(tc, TOKEN_IDENTIFIER, "counter");
	testNextToken(tc, TOKEN_LBRACE, "{");	
	testNextToken(tc, TOKEN_IDENTIFIER, "clk");
	testNextToken(tc, TOKEN_INPUT, "->");
	testNextToken(tc, TOKEN_STL, "stl");
	testNextToken(tc, TOKEN_SCOLON, ";");
	testNextToken(tc, TOKEN_IDENTIFIER, "rst");
	testNextToken(tc, TOKEN_INPUT, "->");
	testNextToken(tc, TOKEN_STL, "stl");
	testNextToken(tc, TOKEN_SCOLON, ";");
	testNextToken(tc, TOKEN_IDENTIFIER, "upDown");
	testNextToken(tc, TOKEN_INPUT, "->");
	testNextToken(tc, TOKEN_STL, "stl");
	testNextToken(tc, TOKEN_SCOLON, ";");
	testNextToken(tc, TOKEN_IDENTIFIER, "Q");
	testNextToken(tc, TOKEN_OUTPUT, "<-");
	testNextToken(tc, TOKEN_STLV, "stlv");
	testNextToken(tc, TOKEN_LPAREN, "(");
	testNextToken(tc, TOKEN_NUMBERLIT, "3");
	testNextToken(tc, TOKEN_DOWNTO, "downto");
	testNextToken(tc, TOKEN_NUMBERLIT, "0");
	testNextToken(tc, TOKEN_RPAREN, ")");
	testNextToken(tc, TOKEN_SCOLON, ";");
	testNextToken(tc, TOKEN_RBRACE, "}");	
	
	free(input);
	FreeLexer();
}

void TestNextToken_SignalWithEdge (CuTest *tc){
	char* input = strdup("\
		proc (clk) {\n \
			if(clk'up){}\n \
		}\n \
	");
	InitLexer(input);

	testNextToken(tc, TOKEN_PROC, "proc");	
	testNextToken(tc, TOKEN_LPAREN, "(");	
	testNextToken(tc, TOKEN_IDENTIFIER, "clk");	
	testNextToken(tc, TOKEN_RPAREN, ")");	
	testNextToken(tc, TOKEN_LBRACE, "{");	
	testNextToken(tc, TOKEN_IF, "if");	
	testNextToken(tc, TOKEN_LPAREN, "(");	
	testNextToken(tc, TOKEN_IDENTIFIER, "clk");	
	testNextToken(tc, TOKEN_TICK, "\'");	
	testNextToken(tc, TOKEN_UP, "up");	
	testNextToken(tc, TOKEN_RPAREN, ")");	
	testNextToken(tc, TOKEN_LBRACE, "{");	
	testNextToken(tc, TOKEN_RBRACE, "}");	
	testNextToken(tc, TOKEN_RBRACE, "}");	
	
	free(input);
	FreeLexer();
}

void TestNextToken_ (CuTest *tc){
	char* input = strdup("");

	InitLexer(input);

	//test case here
	
	free(input);
	FreeLexer();
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
	SUITE_ADD_TEST(suite, TestNextToken_ProcessDeclaration);
	SUITE_ADD_TEST(suite, TestNextToken_Type);
	SUITE_ADD_TEST(suite, TestNextToken_Component);
	SUITE_ADD_TEST(suite, TestNextToken_SignalWithEdge);

	return suite;
}

