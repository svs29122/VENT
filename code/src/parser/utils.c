#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "error.h"
#include "utils.h"

bool match(enum TOKEN_TYPE type){
	return p->currToken.type == type;
}

bool peek(enum TOKEN_TYPE type){
	return p->peekToken.type == type;
}

void consume(enum TOKEN_TYPE type, const char* msg){
	if(!match(type)){
		error(p->currToken.lineNumber, p->currToken.literal, msg);
		
		// first check if next token is the one we expect
		nextToken();
		if(match(type)) return;

		// if not then try the next token
		if(peek(type)) {
			nextToken();
			return;
		}

		// else just roll to the end of a statement, block, or progran
		while(!peek(TOKEN_RBRACE) && !peek(TOKEN_SCOLON) && !peek(TOKEN_EOP) && !match(TOKEN_EOP)){
			nextToken();
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

bool userDefinedDataType(){
	return match(TOKEN_IDENTIFIER) && GetInHashTable(enumTypeTable, p->currToken.literal, NULL);
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

bool endOfProgram(){
	return match(TOKEN_EOP);
}

bool thereAreDesignUnits(){
	bool valid = false;

	valid = 	match(TOKEN_USE)	|| 
				match(TOKEN_ENT)  ||
				match(TOKEN_ARCH);

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

struct ComponentDecl* getComponentFromStore(char* cname){
	struct ComponentDecl* thisComp = NULL;
		
	//find the component corresponding to the instance
	for(int i=0; i<BlockCount(componentStore); i++){
		struct Declaration* decl = (struct Declaration*)ReadBlockArray(componentStore, i);
		if(decl) {
			struct ComponentDecl* comp = &(decl->as.componentDeclaration);
			if(strncmp(comp->name->value, cname, sizeof(cname)) == 0){
				thisComp = comp;
			}
		}
	}

	return thisComp;
}

bool thisIsAGenericMap(struct Expression* map, struct Identifier* name, uint16_t pos){
	struct DynamicBlockArray* generics = NULL;
	struct ComponentDecl* comp = NULL;

	comp = getComponentFromStore(name->value);
	if(comp) generics = comp->generics;

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
			} else {
				printf("Error determining mapping! Map type == %d\r\n", map->type);
			}    
		}
	}

	return false;
}

bool positionalMapping(struct Expression* expr){
	if(expr->type == NAME_EXPR 	||
		expr->type == NUM_EXPR 		||
		expr->type == PHYLIT_EXPR 	||
		expr->type == CHAR_EXPR		|| 
		expr->type == STRING_EXPR) 
			return true;

	return false;
}

bool associativeMapping(struct Expression* expr){
	return expr->type == BINARY_EXPR;
}
