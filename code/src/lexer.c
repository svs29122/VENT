/*
	lexer.c

	this file contains all code relating to transforming (lexing) 
	a VHDL file into a series of tokens that the parser can process 
	--
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "token.h"


struct lexer {
	char *input;
	char ch;

	int currPos;
	int readPos;
	int line;
};

char readChar(struct lexer* l);

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
	*(tok.literal) = literal;

	return tok;
}

static Token readIdentifier(struct lexer *l){
	Token tok;

	return tok;
}

static Token readNumber(struct lexer *l){
	Token tok;

	return tok;
}

static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

static char peek(struct lexer* l){
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
				if(peek(l) == '/') {
					// handle single-line comment
					while(peek(l) != '\n' && peek(l) != '\0') readChar(l);
				} else if (peek(l) == '*'){
					//handle multi-line comment
					while(peek(l) != '*' && peek(l) != '\0'){
						readChar(l);
						if(peek(l) == '/') return;
					}
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

	switch(l->ch){
		case '(' : return newToken(LPAREN, l->ch);
		case ')' : return newToken(RPAREN, l->ch);
		case ':' : return newToken(COLON, l->ch);
		case '{' : return newToken(LBRACE, l->ch);
		case '}' : return newToken(RBRACE, l->ch);
		case ',' : return newToken(COMMA, l->ch);
		case '\'': return newToken(TICK, l->ch);
		case '/' : return newToken(SLASH, l->ch);
		case '*' : return newToken(STAR, l->ch);
		case '-' : return newToken(MINUS, l->ch);
		case '+' : return newToken(PLUS, l->ch);
		case '=' : return newToken(EQUAL, l->ch);
		default:
			if(isLetter(l->ch)){
				return readIdentifier(l); 
			} else if (isNumber(l->ch)){
				return readNumber(l);
			} else {
				return newToken(ILLEGAL, l->ch);
			}
	}
}

