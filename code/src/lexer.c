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
	char *input;
	int currPos;
	int readPos;
	
	char ch;
	int line;
};

struct lexer* initLexer(char* in){
	struct lexer* newLexer = malloc(sizeof(struct lexer));

	newLexer->input = in;
	newLexer->currPos = -1;
	newLexer->readPos = 0;
	newLexer->line = 1;

	return newLexer;
}

int readChar(struct lexer* l){
	
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

void NextToken() {

}
