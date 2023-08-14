#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "internal_parser.h"
	
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

struct Expression* copyExpression(struct Expression* expr){

}

struct Expression* createBinaryExpression(struct Expression* l, char* op, struct Expression* r){ 
   struct BinaryExpr* biexp = calloc(1, sizeof(struct BinaryExpr));
   biexp->self.root.type = AST_EXPRESSION;
   biexp->self.type = BINARY_EXPR;

   biexp->left = l;

   int size = strlen(op) + 1;  
   biexp->op = calloc(size, sizeof(char));
   memcpy(biexp->op, op, size);

   biexp->right = r;
   
   return &(biexp->self);
}

