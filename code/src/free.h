#ifndef INC_FREE_H
#define _INC_FREE_H

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

// I know this isn't portable, but I just love it so much 
#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})

void FreeProgram(struct Program* prog){
	
	// setup block
	struct OperationBlock* opBlk = InitOperationBlock();
	opBlk->doProgOp 			= lambda (void, (void* prog) 	{ struct Program* pg = (struct Program*)prog; pg->useStatements=NULL; pg->units=NULL; free(pg); });
	opBlk->doUseStatementOp = lambda (void, (void* stmt) 	{ struct UseStatement* st = (struct UseStatement*)stmt; if(st->value) free(st->value); });
	opBlk->doBlockArrayOp 	= lambda (void, (void* arr) 	{ FreeBlockArray((Dba*)arr); });
	opBlk->doIdentifierOp	= lambda (void, (void* ident) { struct Identifier* id = (struct Identifier*)ident; if(id->value) free(id->value); free(id); });
	opBlk->doPortModeOp 		= lambda (void, (void* pmode) { struct PortMode* pm = (struct PortMode*)pmode; if(pm->value) free(pm->value); free(pm); });
	opBlk->doDataTypeOp 		= lambda (void, (void* dtype) { struct DataType* dt = (struct DataType*)dtype; if(dt->value) free(dt->value); free(dt); });
	opBlk->doExpressionOp	= freeExpression;
	
	WalkTree(prog, opBlk);

	if(p->currToken.literal) free(p->currToken.literal);
	if(p->peekToken.literal) free(p->peekToken.literal);

	free(opBlk);
}

#endif // INC_FREE_H
