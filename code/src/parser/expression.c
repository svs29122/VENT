#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "internal_parser.h"
#include "expression.h"
	
static struct Expression* copyExpression(struct Expression* oldExpr);

static struct Expression* copyCharExpr(struct CharExpr* orig){
   if(orig == NULL) return NULL;

	struct CharExpr* chexp = calloc(1, sizeof(struct CharExpr));
   chexp->self.root.type = AST_EXPRESSION;
   chexp->self.type = CHAR_EXPR;

   int size = strlen(orig->literal) + 1; 
   chexp->literal = calloc(size, sizeof(char));
   memcpy(chexp->literal, orig->literal, size);
   
   return &(chexp->self);
}

static struct Expression* copyStringExpr(struct StringExpr* orig){
   if(orig == NULL) return NULL;

	struct StringExpr* stexp = calloc(1, sizeof(struct StringExpr));
   stexp->self.root.type = AST_EXPRESSION;
   stexp->self.type = STRING_EXPR;

   int size = strlen(orig->literal) + 1;
   stexp->literal = calloc(size, sizeof(char));
   memcpy(stexp->literal, orig->literal, size);

   return &(stexp->self); 
}

static struct Expression* copyNumExpr(struct NumExpr* orig){
   if(orig == NULL) return NULL;

 	struct NumExpr* nexp = calloc(1, sizeof(struct NumExpr));
   nexp->self.root.type = AST_EXPRESSION;
   nexp->self.type = NUM_EXPR;

   int size = strlen(orig->literal) + 1;
   nexp->literal = calloc(size, sizeof(char));
   memcpy(nexp->literal, orig->literal, size);

	return &(nexp->self);
}

static struct Expression* copyBinaryExpr(struct BinaryExpr* orig){
   if(orig == NULL) return NULL;
	
	struct BinaryExpr* biexp = calloc(1, sizeof(struct BinaryExpr));
   biexp->self.root.type = AST_EXPRESSION;
   biexp->self.type = BINARY_EXPR;

   biexp->left = copyExpression(orig->left);

   int size = strlen(orig->op) + 1;
   biexp->op = calloc(size, sizeof(char));
   memcpy(biexp->op, orig->op, size);

   biexp->right = copyExpression(orig->right);

   return &(biexp->self); 
}

static struct Identifier* copyIdentifier(struct Identifier* orig){
   if(orig == NULL) return NULL;

   struct Identifier* ident = calloc(1, sizeof(struct Identifier));  
   ident->self.root.type = AST_IDENTIFIER;
   ident->self.type = NAME_EXPR;

   int size = strlen(orig->value) + 1;  
   ident->value = calloc(size, sizeof(char));
   memcpy(ident->value, orig->value, size);

   return ident;
}

static struct Expression* copyExpression(struct Expression* oldExpr){
	struct Expression* newExpr = NULL;

	switch(oldExpr->type){

		case BINARY_EXPR: {
			newExpr = copyBinaryExpr((struct BinaryExpr*) oldExpr);
			break;
		}

		case UNARY_EXPR: {
			//TODO: update once parseUnary has been written
			break;
		}

		case NAME_EXPR: {
			newExpr = (struct Expression*)copyIdentifier((struct Identifier*)oldExpr);
			break;
		}

		case CHAR_EXPR: {
			newExpr = copyCharExpr((struct CharExpr*)oldExpr);
			break;
		}

		case STRING_EXPR: {
			newExpr = copyStringExpr((struct StringExpr*)oldExpr);
			break;
		}

		case NUM_EXPR: {
			newExpr = copyNumExpr((struct NumExpr*)oldExpr);
			break;
		}
	
		default:
			printf("Unhandled expression type in copyExpression\r\n");
			break;
	}

	return newExpr;
}

struct Expression* createBinaryExpression(struct Expression* l, char* op, struct Expression* r){ 
   struct BinaryExpr* biexp = calloc(1, sizeof(struct BinaryExpr));
   biexp->self.root.type = AST_EXPRESSION;
   biexp->self.type = BINARY_EXPR;

   biexp->left = copyExpression(l);

   int size = strlen(op) + 1;  
   biexp->op = calloc(size, sizeof(char));
   memcpy(biexp->op, op, size);

   biexp->right = copyExpression(r);
   
   return &(biexp->self);
}

