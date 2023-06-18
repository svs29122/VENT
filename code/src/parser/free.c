#include <stdlib.h>
#include <stddef.h>

#include <ast.h>

#include "parser_internal.h"

static void freeProgram(struct AstNode* prog){
	struct Program* pg = (struct Program*)prog;
	pg->useStatements = NULL;
	pg->units = NULL;

	free(pg);
}

static void freeUse(struct AstNode* stmt){
	struct UseStatement* st = (struct UseStatement*)stmt;
	if(st->value) free(st->value);
}

static void freeIdentifier(struct AstNode* ident){
	struct Identifier* id = (struct Identifier*)ident;
	if(id->value) free(id->value);

	free(id);
}

static void freePortMode(struct AstNode* pmode){
	struct PortMode* pm = (struct PortMode*)pmode;
	if(pm->value) free(pm->value);

	free(pm);
}

static void freeDataType(struct AstNode* dtype){
	struct DataType* dt = (struct DataType*)dtype;
	if(dt->value) free(dt->value);

	free(dt);
}

static void freeAssignmentOp(struct AstNode* vAssign){
	char* op = ((struct VariableAssign*)vAssign)->op;
	free(op);
}

static void freeElsifStatement(struct AstNode* ifStmt){
	struct IfStatement* ifs = (struct IfStatement*)ifStmt;
	free(ifs);
}

static void freeBlockArray(struct DynamicBlockArray* arr){
	FreeBlockArray(arr);
}

static void freeExpression(struct Expression* expr){
   enum ExpressionType type = expr->type;

   switch(type) {

      case CHAR_EXPR: {
         struct CharExpr* chexp = (struct CharExpr*)expr;
         free(chexp->literal);
			free(chexp);
         break;
      }

      case NUM_EXPR: {
         struct NumExpr* nexp = (struct NumExpr*)expr;
         free(nexp->literal);
			free(nexp);
         break;
      }

      case BINARY_EXPR:{
         struct BinaryExpr* bexp = (struct BinaryExpr*) expr;
         freeExpression(bexp->left);
         free(bexp->op);
         freeExpression(bexp->right);
			free(bexp);
         break;
      }

      case NAME_EXPR: {
         //NameExpr* nexp = (NameExpr*) expr;
         //free(nexp->name->value);
         struct Identifier* ident = (struct Identifier*)expr;
         free(ident->value);
         free(ident);
         break;
      }

      default:
         break;
   }
}

static void freeRange(struct AstNode* rng){
	struct Range* range = (struct Range*)rng;

	if(range->left) freeExpression(range->left);
	if(range->right) freeExpression(range->right);

	free(range);
}

static void freeSpecial(struct AstNode* node){

	switch(node->type){

		case AST_PROGRAM:
			freeProgram(node);
			break;

		case AST_ELSIF:
			freeElsifStatement(node);
			break;

		case AST_VASSIGN:
			freeAssignmentOp(node);
			break;

		default:
			break;
	}
}

static void freeDefault(struct AstNode* node){
	
	switch(node->type){

		case AST_USE:
			freeUse(node);
			break;

		case AST_IDENTIFIER:
			freeIdentifier(node);
			break;

		case AST_PMODE:
			freePortMode(node);
			break;

		case AST_DTYPE:
			freeDataType(node);
			break;

		case AST_RANGE:
			freeRange(node);
			break;

		default:
			break;
	}
}

void FreeProgram(struct Program* prog){
	
	// setup block
	struct OperationBlock opBlk = {
		.doDefaultOp 		= freeDefault,
		.doSpecialOp		= freeSpecial,
		.doExpressionOp	= freeExpression,
		.doBlockArrayOp	= freeBlockArray,
	};
	
	WalkTree(prog, &opBlk);

	freeParserTokens();
}
