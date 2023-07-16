#include <stdlib.h>
#include <string.h>

#include <token.h>

#include "utils.h"
#include "error.h"
#include "parser_internal.h"

bool match(enum TOKEN_TYPE type){
	return p->currToken.type == type;
}

bool peek(enum TOKEN_TYPE type){
	return p->peekToken.type == type;
}

void consume(enum TOKEN_TYPE type, const char* msg){
	if(!match(type)){
		error(p->currToken.lineNumber, p->currToken.literal, msg);
		
		// if the next token is the one we expect move to it
		if(peek(type)) {
			nextToken();
		} else {
			while(!peek(TOKEN_RBRACE) && !peek(TOKEN_EOP) && !match(TOKEN_EOP)){
				nextToken();
			}
		}
	}
}

void consumeNext(enum TOKEN_TYPE type, const char* msg){
	nextToken();
	consume(type, msg);
}

struct Token copyToken(struct Token oldToken){
	struct Token newToken = oldToken;

	newToken.literal = calloc(1, sizeof(strlen(oldToken.literal)));
	strncpy(newToken.literal,  oldToken.literal, sizeof(strlen(oldToken.literal)));

	return newToken;
}

void destroyToken(struct Token thisToken){
	free(thisToken.literal);
}

bool validDataType(){
	bool valid = false; 
	
	valid = 	match(TOKEN_STL) 		||	match(TOKEN_STLV) 	||
				match(TOKEN_INTEGER) || match(TOKEN_STRING) 	||
				match(TOKEN_BIT) 		|| match(TOKEN_BITV)		||
				match(TOKEN_SIGNED)	|| match(TOKEN_UNSIGNED);

	return valid;
}

bool validAssignment(){
	bool valid = false;

	valid =	match(TOKEN_SLASH_EQUAL)	||
				match(TOKEN_STAR_EQUAL) 	||
				match(TOKEN_MINUS_EQUAL) 	||
				match(TOKEN_MINUS_MINUS)	||
				match(TOKEN_PLUS_EQUAL)		||
				match(TOKEN_PLUS_PLUS)		||
				match(TOKEN_VASSIGN);
	
	return valid;
}

bool thereAreDeclarations(){
	bool valid = false;

	valid = 	match(TOKEN_SIG)		|| match(TOKEN_VAR)		||
				match(TOKEN_TYPE)		|| match(TOKEN_COMP)		|| 
				match(TOKEN_FILE);

	return valid;
}

