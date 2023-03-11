/*
	parser.c

	This file contains all code used to parse the list of tokens 
	produced by the lexer. The end result is an abstract syntax
	tree representing a VENT program/file.
	--
*/

#include <stdio.h>

#include "token.h"
#include "lexer.h"

struct parser {
	struct lexer* l;
	Token curToken;
	Token peekToken;
};

struct parser* NewParser(struct lexer *lex){
	struct parser* newParser = (struct parser*)malloc(sizeof(struct parser));

	newParser->l = lex;
	newParser->curToken = lex->NextToken();
	newParser->peekToken = lex->NextToken();	

	return newParser;
}
