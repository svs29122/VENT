#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "internal_parser.h"

static void freeRange(struct AstNode* rng);

static void freeProgram(struct AstNode* prog){
	struct Program* pg = (struct Program*)prog;
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

static void freeLabel(struct AstNode* lbl){
	struct Label* label = (struct Label*)lbl;
	if(label->value) free(label->value);

	free(label);
}

static void freePortMode(struct AstNode* pmode){
	struct PortMode* pm = (struct PortMode*)pmode;
	if(pm->value) free(pm->value);

	free(pm);
}

static void freeDataType(struct AstNode* dtype){
	struct DataType* dt = (struct DataType*)dtype;
	if(dt->value) free(dt->value);
	if(dt->range) freeRange((struct AstNode*)dt->range);

	free(dt);
}

static void freeCaseChoices(struct AstNode* caseStmt){
	struct CaseStatement* cstmt = (struct CaseStatement*)caseStmt;
	
	struct Choice* choice = cstmt->choices;
	while(choice != NULL){
		struct Choice* prev = choice;
		choice = choice->nextChoice;
		free(prev);
	}
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

void freeExpression(struct Expression* expr){
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

      case UNARY_EXPR: {
         struct UnaryExpr* uexp = (struct UnaryExpr*)expr;
         free(uexp->op);
         freeExpression(uexp->right);
			free(uexp);
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

      case ATTRIBUTE_EXPR:{
         struct AttributeExpr* aexp = (struct AttributeExpr*) expr;
         freeExpression(aexp->object);
         freeExpression(aexp->attribute);
			free(aexp);
         break;
      }

      case CALL_EXPR:{
         struct CallExpr* cexp = (struct CallExpr*) expr;
         freeExpression(cexp->function);
         freeExpression(cexp->parameters);
			free(cexp);
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
	
		case STRING_EXPR: {
			struct StringExpr* strexp = (struct StringExpr*)expr;
			free(strexp->literal);
			free(strexp);
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

static void freeExpressionList(struct ExpressionNode* head){

	struct ExpressionNode *curr, *prev;
	curr = head;

	while(curr){
		prev = curr;
		curr = curr->next;
		free(prev);
	}
}

static void freeOpen(struct AstNode* node){

	switch(node->type){

		case AST_CASE:
			freeCaseChoices(node);
			break;

		default:
			break;
	}
}

static void freeClose(struct AstNode* node){

	switch(node->type){

		case AST_INSTANCE: {
			struct Instantiation* instance = (struct Instantiation*)node;
			freeExpressionList(instance->portMap);
			freeExpressionList(instance->genericMap);
			break;
		}

		case AST_TDECL: {
			struct TypeDecl* typeDecl = (struct TypeDecl*)node;
			freeExpressionList(typeDecl->enumList);
			break;
		}

		default:
			break;
	}
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

		case AST_LABEL:
			freeLabel(node);
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

static void freeParserTokens(){
   if(p->currToken.literal) free(p->currToken.literal);
   if(p->peekToken.literal) free(p->peekToken.literal);
}

static void freeParserData(){
	FreeBlockArray(componentStore);
	FreeHashTable(enumTypeTable);
}

void FreeProgram(struct Program* prog){
	
	// setup block
	struct OperationBlock opBlk = {
		.doDefaultOp 		= freeDefault,
		.doOpenOp 			= freeOpen,
		.doCloseOp 			= freeClose,
		.doSpecialOp		= freeSpecial,
		.doExpressionOp	= freeExpression,
		.doBlockArrayOp	= freeBlockArray,
	};
	
	WalkTree(prog, &opBlk);

	freeParserTokens();
	freeParserData();
	FreeLexer();
}
