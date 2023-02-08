/*
	lexer.c

	this file contains all code relating to transforming (lexing) 
	a VHDL file into a series of tokens that the parser can process 
	--
*/
#include <stdio.h>
#include <stdlib.h>

#include "token.h"

struct lexer {
	FILE *input;
	int pos;
	int readPos;
	char ch;
};

struct lexer* initLexer(FILE* in){
	struct lexer * newLexer = malloc(sizeof(struct lexer));

	newLexer->input = in;
	newLexer->pos = -1;
	newLexer->readPos = 0;
}

int readChar(struct lexer* lex){
	
	//bump pos and grab the next char
	lex->ch = fgetc(lex->input);

	if(lex->ch == EOF){
		return EOF;
	}
	
	return 0; 
}

char getChar(struct lexer* lex){
	if(lex)
		return lex->ch;
	else
		return -1;
}
