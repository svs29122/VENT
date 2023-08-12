#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <token.h>
#include <ast.h>

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

struct Identifier* copyIdentifier(struct Identifier* orig){
	if(orig == NULL) return NULL;

	struct Identifier* ident = calloc(1, sizeof(struct Identifier));  
	ident->self.root.type = AST_IDENTIFIER;
	ident->self.type = NAME_EXPR;

	int size = strlen(orig->value) + 1; 
	ident->value = calloc(size, sizeof(char));
	memcpy(ident->value, orig->value, size);

	return ident;
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


bool thisIsAPort(){
	bool valid = false;

	valid = 	peek(TOKEN_INPUT) ||
				peek(TOKEN_OUTPUT) ||
				peek(TOKEN_INOUT);

	return valid;
}

bool thisIsAWildCard(struct Expression* map){
	if(map->type == CHAR_EXPR){
		struct CharExpr* charLit = (struct CharExpr*)map;
		if(*(charLit->literal) == '*') {		

			//trash the '*' char as we don't need it anymore
			free(charLit->literal);
			free(charLit);

			return true;	
		}
	}
	return false;
}

bool thisIsAGenericMap(struct Expression* map, struct Identifier* name, uint16_t pos){
	struct DynamicBlockArray* generics = NULL;
	
	//find the component corresponding to the instance
	for(int i=0; i<BlockCount(componentStore); i++){
		struct Declaration* decl = (struct Declaration*)ReadBlockArray(componentStore, i);
		if(decl) {
			struct ComponentDecl* comp = &(decl->as.componentDeclaration);
			if(strncmp(comp->name->value, name->value, sizeof(name->value)) == 0){
				generics = comp->generics;
			}
		}
	}

	if(generics){
		for(int i=0; i<BlockCount(generics); i++){
   		struct GenericDecl* generic = (struct GenericDecl*)ReadBlockArray(generics, i);
			if(positionalMapping(map)){
      		if(generic->position == pos){
					return true;
				}
			} else if (associativeMapping(map)) {
      		struct BinaryExpr* bexp = (struct BinaryExpr*)map;
      		struct Identifier* left = (struct Identifier*)bexp->left;
				if(strncmp(generic->name->value, left->value, sizeof(left->value)) == 0) {
					return true;
				}
			}    
		}
	}

	return false;
}

bool positionalMapping(struct Expression* expr){
	return expr->type == NAME_EXPR;
}

bool associativeMapping(struct Expression* expr){
	return expr->type == BINARY_EXPR;
}
