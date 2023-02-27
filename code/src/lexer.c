/*
	lexer.c

	this file contains all code relating to transforming (lexing) 
	a VHDL file into a series of tokens that the parser can process 
	--
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lexer.h"


struct lexer {
	char *input;
	char ch;

	int currPos;
	int readPos;
	int line;
};

//char readChar(struct lexer* l);

struct lexer* NewLexer(char* in){
	struct lexer* newLexer = (struct lexer*)malloc(sizeof(struct lexer));

	newLexer->input = in;
	newLexer->currPos = -1;
	newLexer->readPos = 0;
	newLexer->line = 1;

	//init our lexer with a char
	readChar(newLexer);

	return newLexer;
}

char readChar(struct lexer* l){
	
	//grab next char in buffer
	l->ch = l->input[l->readPos];
	
	//bump positions	
	l->currPos = l->readPos;
	l->readPos += 1;

	return l->ch;
}

char getChar(struct lexer* l){
	if(l)
		return l->ch;
	else
		return -1;
}

static Token newToken(enum TOKEN_TYPE type, char literal){
	Token tok;

	tok.type = type;
	tok.literal = (char*)malloc(sizeof(char));
	memcpy(tok.literal, &literal, sizeof(char));

	return tok;
}

static Token readIdentifier(struct lexer *l){
	Token tok = {ILLEGAL, 0};
	return tok;
}

static Token readStringLiteral(struct lexer *l){
	Token tok = {ILLEGAL, 0};
	return tok;
}

static Token readNumericLiteral(struct lexer *l){
	Token tok = {ILLEGAL, 0};

	//we've already passed the first char of number so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	while(l->ch != ' ' && l->ch != '\0'){
		readChar(l);
		end++;
	}	

	tok.type = NUMBER;
		
	int lSize = sizeof(char) * (int)(end-start);
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);

	return tok;
}

static bool isCharLiteral(struct lexer *l) {
    return 0;
}

static bool isStringLiteral(struct lexer *l) {
    return 0;
}

static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isNumber(char c) {
    return c >= '0' && c <= '9';
}


static char peek(struct lexer* l){
	return l->input[l->currPos];
}

static char peekNext(struct lexer* l){
	return l->input[l->readPos];
}

static void skipWhiteSpace(struct lexer *l){
	for(;;){
		switch(l->ch){
			case ' ':
			case '\t':
			case '\r':
				readChar(l);
				break;

			case '\n':
				l->line++;
				readChar(l);
				break;	

			case '/':
				if(peekNext(l) == '/') {
					// handle single-line comment
					while(peek(l) != '\n' && peek(l) != '\0'){
						 readChar(l);
					}
				} else if (peekNext(l) == '*'){
					//handle multi-line comment
					readChar(l);
					readChar(l);
					while((peek(l) != '*' || peekNext(l) != '/') && peek(l) != '\0'){
						readChar(l);
					}
					readChar(l);
					readChar(l);
				} else {
					return;
				}
				break;

			 default:
				return;
		}
	}
}

Token NextToken(struct lexer* l) {
	
	skipWhiteSpace(l);

	char ch = l->ch;
	readChar(l);

	switch(ch){
		case '(' : return newToken(LPAREN, ch);
		case ')' : return newToken(RPAREN, ch);
		case ':' : return newToken(COLON, ch);
		case '{' : return newToken(LBRACE, ch);
		case '}' : return newToken(RBRACE, ch);
		case ',' : return newToken(COMMA, ch);
		case '/' : return newToken(SLASH, ch);
		case '*' : return newToken(STAR, ch);
		case '-' : return newToken(MINUS, ch); //will need to handle unary - (negative) operator
		case '+' : return newToken(PLUS, ch);
		case '=' : return newToken(EQUAL, ch);
		case '\'':
			if(isCharLiteral(l)){
				// handle char literal
			} else {
				return newToken(TICK, ch);
			}
		case '"':
		case 'B':
		case 'O':
		case 'X':
			if(isStringLiteral(l)){
				return readStringLiteral(l);
			} // else must be identifier. Falling through to default!!! 
		default:
			if(isLetter(ch)){
				return readIdentifier(l); 
			} else if (isNumber(ch)){
				return readNumericLiteral(l);
			} else {
				return newToken(ILLEGAL, ch);
			}
	}
}

void PrintToken(Token t){
	printf("type: %d, literal: %c\n", t.type, *(t.literal));
}
