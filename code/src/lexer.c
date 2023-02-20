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
					while(peek(l) != '*' && peek(l) != '\0'){
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
		case '\'': return newToken(TICK, ch);
		case '/' : return newToken(SLASH, ch);
		case '*' : return newToken(STAR, ch);
		case '-' : return newToken(MINUS, ch);
		case '+' : return newToken(PLUS, ch);
		case '=' : return newToken(EQUAL, ch);
		default:
			if(isLetter(ch)){
				return readIdentifier(l); 
			} else if (isNumber(ch)){
				return readNumber(l);
			} else {
				return newToken(ILLEGAL, ch);
			}
	}
}

void PrintToken(Token t){
	printf("type: %d, literal: %c\n", t.type, *(t.literal));
}
