#ifndef INC_FREE_H
#define INC_FREE_H

#define lambda(function_body) \
({ \
      void __fn__ function_body \
          __fn__; \
})

static void freeProgram(void* prog){
	struct Program* pg = (struct Program*)prog;
	pg->useStatements = NULL;
	pg->units = NULL;

	free(pg);
}

static void freeUse(void* stmt){
	struct UseStatement* st = (struct UseStatement*)stmt;
	if(st->value) free(st->value);
}

static void freeIdentifier(void* ident){
	struct Identifier* id = (struct Identifier*)ident;
	if(id->value) free(id->value);

	free(id);
}

static void freePortMode(void* pmode){
	struct PortMode* pm = (struct PortMode*)pmode;
	if(pm->value) free(pm->value);

	free(pm);
}

static void freeDataType(void* dtype){
	struct DataType* dt = (struct DataType*)dtype;
	if(dt->value) free(dt->value);

	free(dt);
}

static void freeAssignmentOp(void* vAssign){
	char* op = ((struct VariableAssign*)vAssign)->op;
	free(op);
}

static void freeElsifStatement(void* ifStmt){
	struct IfStatement* ifs = (struct IfStatement*)ifStmt;
	free(ifs);
}

static void freeBlockArray(void* arr){
	FreeBlockArray((Dba*)arr);
}

static void freeExpression(void* expr){
   enum ExpressionType type = {0};
	type = ((struct Expression*)expr)->type;

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
         freeExpression((void*)bexp->left);
         free(bexp->op);
         freeExpression((void*)bexp->right);
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

static void freeRange(void* rng){
	struct Range* range = (struct Range*)rng;

	if(range->left) freeExpression(range->left);
	if(range->right) freeExpression(range->right);

	free(range);
}

static void freeSpecial(struct AstNode* node){

	switch(node->type){

		case AST_PROGRAM:
			freeProgram((void*)node);
			break;

		case AST_ELSIF:
			freeElsifStatement((void*)node);
			break;

		case AST_VASSIGN:
			freeAssignmentOp((void*)node);
			break;

		default:
			break;
	}
}

static void freeDefault(struct AstNode* node){
	
	switch(node->type){

		case AST_USE:
			freeUse((void*)node);
			break;

		case AST_IDENTIFIER:
			freeIdentifier((void*)node);
			break;

		case AST_PMODE:
			freePortMode((void*)node);
			break;

		case AST_DTYPE:
			freeDataType((void*)node);
			break;

		case AST_RANGE:
			freeRange((void*)node);
			break;

		default:
			break;
	}
}

void FreeProgram(struct Program* prog){
	
	// setup block
	struct OperationBlock* opBlk = InitOperationBlock();
	opBlk->doDefaultOp				= freeDefault;
	opBlk->doSpecialOp				= freeSpecial;
	opBlk->doBlockArrayOp 			= freeBlockArray;
	opBlk->doExpressionOp			= freeExpression;
	
	WalkTree(prog, opBlk);

	if(p->currToken.literal) free(p->currToken.literal);
	if(p->peekToken.literal) free(p->peekToken.literal);

	free(opBlk);
}

#endif // INC_FREE_H
