/*
	lexer.c

	This file contains all code relating to transforming (lexing) 
	a VENT file into a series of tokens that the parser can process 
	--
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lexer.h"

static char readChar();

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

	//init our lexer with a char
	readChar();
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

// 'get' utilities
static enum TOKEN_TYPE checkKeyword(int start, int keyLen, int litLen, const char* lit, const char* rest, enum TOKEN_TYPE type){
	
	if(litLen == start + keyLen && memcmp(&lit[start], rest, keyLen) == 0){
		return type;
	}

	return TOKEN_IDENTIFIER;
}

static enum TOKEN_TYPE getIdentifierType(int size, char* lit){
	
	switch(lit[0]){
		case 'a':
			if(size > 1){
				switch(lit[1]){
					case 'r':	return checkKeyword(2, 2, size, lit, "ch", TOKEN_ARCH);
					case 'n':	return checkKeyword(2, 1, size, lit, "d", TOKEN_AND);
				}
			}
			break;
		case 'e': return checkKeyword(1, 2, size, lit, "nt", TOKEN_ENT);
		case 'i': return checkKeyword(1, 2, size, lit, "nt", TOKEN_INTEGER);
		case 's': 
			if(size > 1){
				switch(lit[1]){
					case 'i':
						if(size > 2){
							switch(lit[2]){
								case 'g':
									if(size > 3) return checkKeyword(3, 3, size, lit, "ned", TOKEN_SIGNED);
									else return TOKEN_SIG;
							}
						}
						break;
					case 't':
						if(size > 2){
							switch(lit[2]){
								case 'l':
									if(size > 3) return checkKeyword(3, 1, size, lit, "v", TOKEN_STLV);
									else return TOKEN_STL;
								case 'r': 
									if(size > 3) return checkKeyword(3, 3, size, lit, "ing", TOKEN_STRING); 
							}
						}
						break;
				}
			}
			break;
		case 'u':
			if(size > 1){
				switch(lit[1]){
					case 's':
						if(size > 2) return checkKeyword(2, 1, size, lit, "e", TOKEN_USE);
						break;
					case 'n':
						if(size > 2) return checkKeyword(2, 6, size, lit, "signed", TOKEN_UNSIGNED);
						break;
				}
			}
			break;
	}

	return TOKEN_IDENTIFIER;
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

static struct Token readIdentifier(){
	struct Token tok = {TOKEN_ILLEGAL, 0};

	//we've already passed the first char of
	//identifier  so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	//check for any chars that can follow an identifier
	//TODO: this is gonna be huge, maybe break it out
	//to a separate function or a few different functions?
	
	if(peek() != ' ' && peek() != ';') {	
		while(peekNext() != ' ' && peekNext() != '=' &&
				peekNext() != '{' && peekNext() != '}' &&
				peekNext() != '(' && peekNext() != ')' &&
				peekNext() != '<' && peekNext() != '>' &&
				peekNext() != '+' && peekNext() != '-' &&
				peekNext() != '*' && peekNext() != '/' &&
				peekNext() != ';' && peekNext() != ',' &&
				peekNext() != '\n' && peekNext() != '\0'){
	
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
	tok.type = getIdentifierType(lSize-1, tok.literal);
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
	
	int lSize = sizeof(char) * (int)(end-start) + 2;
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

	//TODO: assuming a space following the number?
	// no way that's correct
	while(peek() != ' ' && peek() != '\0'){
		readChar();
		end++;
	}	

	tok.type = TOKEN_NUMBERLIT;
	tok.lineNumber = l->line;
		
	int lSize = sizeof(char) * (int)(end-start) + 2;
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
						 readChar();
					}
				} else if (peekNext() == '*'){
					//handle multi-line comment
					readChar();
					readChar();
					while((peek() != '*' || peekNext() != '/') && peek() != '\0'){
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
		case '/' : return newToken(TOKEN_SLASH, ch);
		case '*' : return newToken(TOKEN_STAR, ch);
		case '-' :
			if(peek() == '>') return newMultiCharToken(TOKEN_INPUT, 2); 			
			return newToken(TOKEN_MINUS, ch);
		case '<' :
			if(peek() == '-') {
				if(peekNext() == '>') return newMultiCharToken(TOKEN_INOUT, 3);
				return newMultiCharToken(TOKEN_OUTPUT, 2);
			} else if(peek() == '='){
				return newMultiCharToken(TOKEN_SASSIGN, 2);
			} 
			return newToken(TOKEN_LESS, ch);
		case '+' : return newToken(TOKEN_PLUS, ch);
		case '=' : 
			if(peek() == '>') return newMultiCharToken(TOKEN_AASSIGN, 2); 			
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
		case TOKEN_SLASH:			return "TOKEN_SLASH";
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
		case TOKEN_SASSIGN: 		return "TOKEN_SASSIGN";
		case TOKEN_VASSIGN: 		return "TOKEN_VASSIGN";
		case TOKEN_AASSIGN: 		return "TOKEN_AASSIGN";
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
		case TOKEN_IDENTIFIER:	return "TOKEN_IDENTIFIER";
		case TOKEN_CHARLIT: 		return "TOKEN_CHARLIT";
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
		case TOKEN_WAIT:	 		return "TOKEN_WAIT";
		case TOKEN_EOP:	 		return "TOKEN_EOP";
		case TOKEN_ILLEGAL: 		return "TOKEN_ILLEGAL";
		default: 			return "";
	}
}

void PrintToken(struct Token t){
	printf("\e[0;35mtype:\e[0m %10s, \e[0;33mliteral:\e[0m %s\n", TokenToString(t.type), t.literal);
}
