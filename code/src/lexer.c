/*
	lexer.c

	This file contains all code relating to transforming (lexing) 
	a VENT file into a series of tokens that the parser can process 
	--
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include <token.h>
#include <dht.h>

static char readChar();
static void initializeKeywordMap();

struct lexer {
	char *input;
	char ch;

	int currPos;
	int readPos;
	
	int line;
	int length;
} static lexer;

static struct lexer *l = &lexer;

void InitLexer(char* in){

	memset(l, 0, sizeof(struct lexer));

	l->input = in;
	l->currPos = -1;
	l->readPos = 0;
	l->line = 1;
	l->length = strlen(in) + 1;
	
	initializeKeywordMap();

	//init our lexer with a char
	readChar();
}

static struct DynamicHashTable* keywordMap = NULL;

void FreeLexer(){
	FreeHashTable(keywordMap);
}

static void initializeKeywordMap(){
	keywordMap = InitHashTable();
	
	//add all VENT keywords to map
	SetInHashTable(keywordMap, "and", TOKEN_AND);
	SetInHashTable(keywordMap, "arch", TOKEN_ARCH);
	SetInHashTable(keywordMap, "assert", TOKEN_ASSERT);
	SetInHashTable(keywordMap, "case", TOKEN_CASE);
	SetInHashTable(keywordMap, "comp", TOKEN_COMP);
	SetInHashTable(keywordMap, "default", TOKEN_DEFAULT);
	SetInHashTable(keywordMap, "down", TOKEN_DOWN);
	SetInHashTable(keywordMap, "downto", TOKEN_DOWNTO);
	SetInHashTable(keywordMap, "else", TOKEN_ELSE);
	SetInHashTable(keywordMap, "elsif", TOKEN_ELSIF);
	SetInHashTable(keywordMap, "ent", TOKEN_ENT);
	SetInHashTable(keywordMap, "error", TOKEN_ERROR);
	SetInHashTable(keywordMap, "failure", TOKEN_FAILURE);
	SetInHashTable(keywordMap, "for", TOKEN_FOR);
	SetInHashTable(keywordMap, "if", TOKEN_IF);
	SetInHashTable(keywordMap, "int", TOKEN_INTEGER);
	SetInHashTable(keywordMap, "loop", TOKEN_LOOP);
	SetInHashTable(keywordMap, "map", TOKEN_MAP);
	SetInHashTable(keywordMap, "not", TOKEN_NOT);
	SetInHashTable(keywordMap, "note", TOKEN_NOTE);
	SetInHashTable(keywordMap, "null", TOKEN_NULL);
	SetInHashTable(keywordMap, "or", TOKEN_OR);
	SetInHashTable(keywordMap, "proc", TOKEN_PROC);
	SetInHashTable(keywordMap, "rem", TOKEN_REM);
	SetInHashTable(keywordMap, "report", TOKEN_REPORT);
	SetInHashTable(keywordMap, "rol", TOKEN_ROL);
	SetInHashTable(keywordMap, "ror", TOKEN_ROR);
	SetInHashTable(keywordMap, "severity", TOKEN_SEVERITY);
	SetInHashTable(keywordMap, "signed", TOKEN_SIGNED);
	SetInHashTable(keywordMap, "sig", TOKEN_SIG);
	SetInHashTable(keywordMap, "sll", TOKEN_SLL);
	SetInHashTable(keywordMap, "sra", TOKEN_SRA);
	SetInHashTable(keywordMap, "srl", TOKEN_SRL);
	SetInHashTable(keywordMap, "stlv", TOKEN_STLV);
	SetInHashTable(keywordMap, "stl", TOKEN_STL);
	SetInHashTable(keywordMap, "string", TOKEN_STRING);
	SetInHashTable(keywordMap, "switch", TOKEN_SWITCH);
	SetInHashTable(keywordMap, "to", TOKEN_TO);
	SetInHashTable(keywordMap, "type", TOKEN_TYPE);
	SetInHashTable(keywordMap, "unsigned", TOKEN_UNSIGNED);
	SetInHashTable(keywordMap, "up", TOKEN_UP);
	SetInHashTable(keywordMap, "use", TOKEN_USE);
	SetInHashTable(keywordMap, "var", TOKEN_VAR);
	SetInHashTable(keywordMap, "wait", TOKEN_WAIT);
	SetInHashTable(keywordMap, "warning", TOKEN_WARNING);
	SetInHashTable(keywordMap, "while", TOKEN_WHILE);
	SetInHashTable(keywordMap, "xor", TOKEN_XOR);
}

static enum TOKEN_TYPE getIdentifierType(char* lit){
	uint64_t largeType = 0;
	enum TOKEN_TYPE type = TOKEN_IDENTIFIER;

	if(GetInHashTable(keywordMap, lit, &largeType)){
		type = (enum TOKEN_TYPE)largeType;
	}

	return type;
}

static struct Token newToken(enum TOKEN_TYPE type, char literal){
	struct Token tok;

	tok.type = type;
	tok.lineNumber = l->line;

	tok.literal = (char*)malloc(sizeof(char) * 2);
	tok.literal[0] = literal;
	tok.literal[1] = '\0';

	return tok;
}

static struct Token newMultiCharToken(enum TOKEN_TYPE type, int len){
	struct Token tok = {TOKEN_ILLEGAL, 0};

	//we've already passed the first char of token  so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	for(int i = 1; i < len-1; i++){
		readChar();
		end++;
	}	

	//move the lexer past the token
	readChar();

	int lSize = sizeof(char) * (int)(end-start) + 2;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize-1] = '\0';

	tok.type = type;
	tok.lineNumber = l->line;

	return tok;
}

static char peek(){
	return l->input[l->currPos];
}

static char peekNext(){
	return l->input[l->readPos];
}

// 'read' utilities
static char readChar(){
	
	//grab next char in buffer
	if(l->readPos < l->length)	
		l->ch = l->input[l->readPos];
	
	//bump positions	
	l->currPos = l->readPos;
	l->readPos += 1;

	return l->ch;
}

#define INVALID_CHAR_NUM 25
char invalidCharacters[INVALID_CHAR_NUM] = { 
   ' ', ';', '=', '{', '}',
   '(', ')', '<', '>', '+',
   '-', '*', '/', ',', '`',
   '~', '!', '&', '?', '"',
	':', '\'', '\n', '\0', '@',
};

static bool charIsValidInIdentifier(char character){
   for(int i=0; i < INVALID_CHAR_NUM; i++){
      if( invalidCharacters[i]  == character) return false;
   }   
   return true;
}
#undef INVALID_CHAR_NUM


static struct Token readIdentifier(){
	struct Token tok = {TOKEN_ILLEGAL, 0};

	//we've already passed the first char of
	//identifier  so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	if(charIsValidInIdentifier(peek())){
		while(charIsValidInIdentifier(peekNext())){
			readChar();
			end++;
		}
	} else {
		//got a single letter identifier
		end--;
	}

	//step the lexer past the identifier
	if(start != end) readChar();	

	int lSize = sizeof(char) * (int)(end-start) + 2;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize-1] = '\0';

#ifdef DEBUG 
	printf("DEBUG: identifer == %s\r\n", tok.literal); 
#endif
	tok.type = getIdentifierType(tok.literal);
	tok.lineNumber = l->line;

	return tok;
}

static struct Token readStringLiteral(){
	struct Token tok = {TOKEN_ILLEGAL, 0};

	//we've already passed the first " of the string
	// so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	while(peek() != '"' && peek() != '\0'){
		readChar();
		end++;
	}	

	//add the end quote too
	if(peek() != '\0'){
		readChar();
		end++;
	}

	tok.type = TOKEN_STRINGLIT;
	tok.lineNumber = l->line;
	
	int lSize = sizeof(char) * (int)(end-start) + 1;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize-1] = '\0';

	return tok;
}

static struct Token readBitStringLiteral(){
	struct Token tok = {TOKEN_ILLEGAL, 0};

	//we've already passed the first base char of 
	//the bitstring so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	//add the start quote
	if(peek() != '\0'){
		readChar();
		end++;
	}

	while(peek() != '"' && peek() != '\0'){
		readChar();
		end++;
	}	

	//add the end quote too
	if(peek() != '\0'){
		readChar();
		end++;
	}

	tok.type = TOKEN_BSTRINGLIT;
	tok.lineNumber = l->line;
	
	int lSize = sizeof(char) * (int)(end-start) + 2;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize-1] = '\0';

	return tok;
}

static struct Token readNumericLiteral(){
	struct Token tok = {TOKEN_ILLEGAL, 0};

	//we've already passed the first char of number so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	//TODO: This is probably still wrong. We may at least need to check ordering.
	while((peek() >= '0' && peek() <= '9') ||  peek() == '.' || peek() == 'E' || peek() == '-'){
		readChar();
		end++;
	}	

	tok.type = TOKEN_NUMBERLIT;
	tok.lineNumber = l->line;
	
	int lSize = sizeof(char) * (int)(end-start) + 1; //add 1 for NULL termination
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize-1] = '\0';

	return tok;
}

static struct Token readCharLiteral(){
	
	char literal = peek();
	struct Token tok = newToken(TOKEN_CHARLIT, literal);		
	
	//move lexer past literal and '
	readChar();
	readChar();

	return tok;
}

// 'is' utilities
static bool isCharLiteral() {
   return peekNext() == '\'';
}

static bool isBitStringLiteral() {
   return peek() == '"';
}

static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

static void skipWhiteSpace(){
	for(;;){
		switch(l->ch){
			case ' ':
			case '\t':
			case '\r':
				readChar();
				break;

			case '\n':
				l->line++;
				readChar();
				break;	

			case '/':
				if(peekNext() == '/') {
					// handle single-line comment
					while(peek() != '\n' && peek() != '\0'){
						l->line++;
						readChar();
					}
					if(peek() == '\n') l->line++;
				} else if (peekNext() == '*'){
					//handle multi-line comment
					readChar();
					readChar();
					while((peek() != '*' || peekNext() != '/') && peek() != '\0'){
						if(peek() == '\n') l->line++;
						readChar();
					}
					readChar();
					readChar();
				} else {
					return;
				}
				break;

			 default:
				return;
		}
	}
}

struct Token NextToken() {
	
	skipWhiteSpace();

	
	char ch = l->ch;
	if(ch == '\0') return newToken(TOKEN_EOP, ch);

	readChar();

	switch(ch){
		case '(' : return newToken(TOKEN_LPAREN, ch);
		case ')' : return newToken(TOKEN_RPAREN, ch);
		case ':' : 
			if(peek() == '=') return newMultiCharToken(TOKEN_VASSIGN, 2);
			return newToken(TOKEN_COLON, ch);
		case ';' : return newToken(TOKEN_SCOLON, ch);
		case '{' : return newToken(TOKEN_LBRACE, ch);
		case '}' : return newToken(TOKEN_RBRACE, ch);
		case ',' : return newToken(TOKEN_COMMA, ch);
		case '|' : return newToken(TOKEN_BAR, ch);
		case '!' : 
			if(peek() == '=') return newMultiCharToken(TOKEN_NOT_EQUAL, 2); 			
		case '/' : 
			if(peek() == '=') return newMultiCharToken(TOKEN_SLASH_EQUAL, 2); 			
			return newToken(TOKEN_SLASH, ch);
		case '*' : 
			if(peek() == '=') return newMultiCharToken(TOKEN_STAR_EQUAL, 2); 			
			return newToken(TOKEN_STAR, ch);
		case '+' : 
			if(peek() == '=') return newMultiCharToken(TOKEN_PLUS_EQUAL, 2); 			
			if(peek() == '+') return newMultiCharToken(TOKEN_PLUS_PLUS, 2); 			
			return newToken(TOKEN_PLUS, ch);
		case '-' :
			if(peek() == '>') return newMultiCharToken(TOKEN_INPUT, 2); 			
			if(peek() == '=') return newMultiCharToken(TOKEN_MINUS_EQUAL, 2); 			
			if(peek() == '-') return newMultiCharToken(TOKEN_MINUS_MINUS, 2); 			
			return newToken(TOKEN_MINUS, ch);
		case '<' :
			if(peek() == '-') {
				if(peekNext() == '>') return newMultiCharToken(TOKEN_INOUT, 3);
				return newMultiCharToken(TOKEN_OUTPUT, 2);
			} else if(peek() == '='){
				return newMultiCharToken(TOKEN_LESS_EQUAL, 2);
			} 
			return newToken(TOKEN_LESS, ch);
		case '>':
			if(peek() == '=') return newMultiCharToken(TOKEN_GREATER_EQUAL, 2);
			return newToken(TOKEN_GREATER, ch);
		case '=' : 
			if(peek() == '>') return newMultiCharToken(TOKEN_MASSIGN, 2); 			
			if(peek() == '=') return newMultiCharToken(TOKEN_EQUAL, 2); 			
			return newToken(TOKEN_EQUAL, ch); 			
		case '\'':
			if(isCharLiteral()) return readCharLiteral();
			return newToken(TOKEN_TICK, ch);
		case '"': return readStringLiteral();
		case 'B':
		case 'O':
		case 'X':
			if(isBitStringLiteral()){
				return readBitStringLiteral();
			} // else must be identifier. Falling through to default!!! 
		default:
			if(isLetter(ch)){
				return readIdentifier(); 
			} else if (isNumber(ch)){
				return readNumericLiteral();
			} else {
				return newToken(TOKEN_ILLEGAL, ch);
			}
	}
}

const char* TokenToString(enum TOKEN_TYPE type){
	switch(type){
		case TOKEN_LPAREN: 		return "TOKEN_LPAREN";
		case TOKEN_RPAREN:		return "TOKEN_RPAREN";
		case TOKEN_COLON:			return "TOKEN_COLON";
		case TOKEN_SCOLON: 		return "TOKEN_SCOLON";
		case TOKEN_LBRACE:		return "TOKEN_LBRACE";
		case TOKEN_RBRACE:		return "TOKEN_RBRACE";
		case TOKEN_COMMA:			return "TOKEN_COMMA";
		case TOKEN_TICK:			return "TOKEN_TICK";
		case TOKEN_BAR:			return "TOKEN_BAR";
		case TOKEN_UP:				return "TOKEN_UP";
		case TOKEN_DOWN:			return "TOKEN_DOWN";
		case TOKEN_SLASH:			return "TOKEN_SLASH";
		case TOKEN_TO:				return "TOKEN_TO";
		case TOKEN_DOWNTO:		return "TOKEN_DOWNTO";
		case TOKEN_STAR:			return "TOKEN_STAR";
		case TOKEN_MINUS: 		return "TOKEN_MINUS";
		case TOKEN_PLUS:	 		return "TOKEN_PLUS";
		case TOKEN_EQUAL: 		return "TOKEN_EQUAL";
		case TOKEN_NOT_EQUAL:	return "TOKEN_NOT_EQUAL"; 
		case TOKEN_GREATER: 		return "TOKEN_GREATER";
		case TOKEN_LESS:	 		return "TOKEN_LESS";
		case TOKEN_AND: 			return "TOKEN_AND";
		case TOKEN_OR: 			return "TOKEN_OR";
		case TOKEN_XOR: 			return "TOKEN_XOR";
		case TOKEN_NOT: 			return "TOKEN_NOT";
		case TOKEN_INPUT: 		return "TOKEN_INPUT";
		case TOKEN_OUTPUT: 		return "TOKEN_OUTPUT";
		case TOKEN_INOUT: 		return "TOKEN_INOUT";
		case TOKEN_SASSIGN: 		
		case TOKEN_LESS_EQUAL:	return "TOKEN_LESS_EQUAL";
		case TOKEN_VASSIGN: 		return "TOKEN_VASSIGN";
		case TOKEN_MASSIGN: 		return "TOKEN_MASSIGN";
		case TOKEN_STL:	 		return "TOKEN_STL";
		case TOKEN_STLV:	 		return "TOKEN_STLV";
		case TOKEN_SIG:	 		return "TOKEN_SIG";
		case TOKEN_VAR:	 		return "TOKEN_VAR";
		case TOKEN_INTEGER: 		return "TOKEN_INTEGER";
		case TOKEN_STRING: 		return "TOKEN_STRING";
		case TOKEN_BIT:	 		return "TOKEN_BIT";
		case TOKEN_BITV: 			return "TOKEN_BITV";
		case TOKEN_SIGNED: 		return "TOKEN_SIGNED";
		case TOKEN_UNSIGNED: 	return "TOKEN_UNSIGNED";
		case TOKEN_TYPE: 			return "TOKEN_TYPE";
		case TOKEN_IDENTIFIER:	return "TOKEN_IDENTIFIER";
		case TOKEN_CHARLIT: 		return "TOKEN_CHARLIT";
		case TOKEN_NUMBERLIT:	return "TOKEN_NUMBERLIT";
		case TOKEN_STRINGLIT:	return "TOKEN_STRINGLIT";
		case TOKEN_BSTRINGLIT:	return "TOKEN_BSTRINGLIT";
		case TOKEN_ENT:	 		return "TOKEN_ENT";
		case TOKEN_ARCH:	 		return "TOKEN_ARCH";
		case TOKEN_GEN:	 		return "TOKEN_GEN";
		case TOKEN_COMP:	 		return "TOKEN_COMP";
		case TOKEN_MAP:	 		return "TOKEN_MAP";
		case TOKEN_PROC: 			return "TOKEN_PROC";
		case TOKEN_OTHER: 		return "TOKEN_OTHER";
		case TOKEN_IF: 			return "TOKEN_IF";
		case TOKEN_ELSIF: 		return "TOKEN_ELSIF";
		case TOKEN_FOR:	 		return "TOKEN_FOR";
		case TOKEN_USE:	 		return "TOKEN_USE";
		case TOKEN_WHILE: 		return "TOKEN_WHILE";
		case TOKEN_LOOP: 			return "TOKEN_LOOP";
		case TOKEN_WAIT:	 		return "TOKEN_WAIT";
		case TOKEN_EOP:	 		return "TOKEN_EOP";
		case TOKEN_ASSERT:	 	return "TOKEN_ASSERT";
		case TOKEN_REPORT:	 	return "TOKEN_REPORT";
		case TOKEN_SEVERITY:	 	return "TOKEN_SEVERITY";
		case TOKEN_SWITCH:	 	return "TOKEN_SWITCH";
		case TOKEN_CASE:	 		return "TOKEN_CASE";
		case TOKEN_ILLEGAL: 		return "TOKEN_ILLEGAL";
		default: 			return "";
	}
}

void PrintToken(struct Token t){
	printf("\e[0;35mtype:\e[0m %-20s \e[0;33mliteral:\e[0m %s\n", TokenToString(t.type), t.literal);
}
