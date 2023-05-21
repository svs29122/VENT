#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <ast.h>

void PrintUsage(void){
	printf("Usage:\n"
			" tvt adder.vent (perform transpilation)\n"
			" tvt adder.vent --print-tokens\n"
			" tvt adder.vent --print-ast\n"
		);
}

static int indent;

static char shift(){
	printf("\e[0;34m|");
	for(int i=0; i<indent; i++){
		printf("-");
	}
	printf("\e[0m");
	return ' ';
}

static void printClose(void* none){
	indent--;
}

static void printProg(){
	printf("\e[1;32m""Program\r\n");
}

static void printUseStatement(void* stmt){
	indent = 0;
	printf("\e[1;36m""%cUseStatement: %s\r\n",shift(), ((struct UseStatement*)stmt)->value);
}

static void printDesignUnit(void* unit){
	indent = 0;
	printf("\e[1;32m""%cDesignUnit\r\n", shift());
	indent++;
}

static void printEntityDecl(void* eDecl){
	printf("\e[0;32m""%cEntityDecl\r\n", shift());
	indent++;
}

static void printArchDecl(void* aDecl){
	printf("\e[0;32m""%cArchDecl\r\n", shift());
	indent++;
}

static void printIdentifier(void* ident){
	printf("\e[0;35m""%cIdentifier: \'%s\'\r\n", shift(), ((struct Identifier*)ident)->value);
}

static void printPortDecl(void* pDecl){
	printf("\e[0;32m""%cPortDecl\r\n", shift());
	indent++;
}

static void printSignalDecl(void* sDecl){
	printf("\e[0;32m""%cSignalDecl\r\n", shift());
	indent++;
}

static void printVariableDecl(void* vDecl){
	printf("\e[0;32m""%cVariableDecl\r\n", shift());
	indent++;
}

static void printIfStatement(void* ifStmt){
	printf("\e[0;33m""%cIfStatement\r\n", shift());
	indent++;
}

static void printElseClause(void* ifStmt){
	printf("\e[0;34m""%c/*ElseBlock*/\r\n", shift());
}

static void printLoopStatement(void* wStmt){
	printf("\e[0;33m""%cLoopStatement\r\n", shift());
	indent++;
}

static void printWhileStatement(void* wStmt){
	printf("\e[0;33m""%cWhileStatement\r\n", shift());
	indent++;
}

static void printWaitStatement(void* wStmt){
	printf("\e[0;33m""%cWaitStatement\r\n", shift());
}

static void printVariableAssign(void* vAssign){
	printf("\e[0;36m""%cVariableAssign\r\n", shift());
	indent++;
}

static void printSignalAssign(void* sAssign){
	printf("\e[0;36m""%cSignalAssign\r\n", shift());
	indent++;
}

static void printProcessStatement(void* proc){
	indent = 2;
	printf("\e[0;33m""%cProcess\r\n", shift());
	indent++;
}

static void printPortMode(void* pMode){
	printf("\e[0;35m""%cPortMode: \'%s\'\r\n", shift(), ((struct PortMode*)pMode)->value);
}

static void printDataType(void* dType){
	printf("\e[0;35m""%cDataType: \'%s\'\r\n", shift(), ((struct DataType*)dType)->value);
}

static void printSubExpression(void* expr){
	enum ExpressionType type = ((struct Expression*)expr)->type;

	switch(type) {
	
		case CHAR_EXPR: {
			struct CharExpr* chexp = (struct CharExpr*)expr;
			printf("%s", chexp->literal);
			break;
		}

		case NUM_EXPR: {
			struct NumExpr* nexp = (struct NumExpr*)expr;
			printf("%s", nexp->literal);
			break;
		}

		case BINARY_EXPR:{
			struct BinaryExpr* bexp = (struct BinaryExpr*) expr;
			printSubExpression((void*)bexp->left);
			printf(" %s ", bexp->op);
			printSubExpression((void*)bexp->right);
			break;
		}

		case NAME_EXPR: {
			//NameExpr* nexp = (NameExpr*) expr;
			//printf("\e[0;35m""\'%s\'\r\n", nexp->name->value);
			struct Identifier* ident = (struct Identifier*)expr;
			printf("%s", ident->value);
			break;
		}

		default:
			break;
	}
}

static void printExpression(void* expr){
	printf("\e[0;35m""%cExpression: \'", shift());
	
	printSubExpression(expr);	
	printf("\'\r\n");
}

static void setupDisplayOpBlock(struct OperationBlock* opBlk){
	opBlk->doUseStatementOp 			= printUseStatement;	
	opBlk->doDesignUnitOp 				= printDesignUnit;
	opBlk->doEntityDeclOp 				= printEntityDecl;
	opBlk->doArchDeclOp 					= printArchDecl;
	opBlk->doPortDeclOp 					= printPortDecl;
	opBlk->doSignalDeclOp 				= printSignalDecl;
	opBlk->doVariableDeclOp 			= printVariableDecl;
	opBlk->doSignalAssignOp 			= printSignalAssign;
	opBlk->doVariableAssignOp 			= printVariableAssign;
	opBlk->doIfStatementOp 				= printIfStatement;
	opBlk->doIfStatementElseOp			= printElseClause;
	opBlk->doLoopStatementOp 			= printLoopStatement;
	opBlk->doWaitStatementOp 			= printWaitStatement;
	opBlk->doWhileStatementOp 			= printWhileStatement;
	opBlk->doProcessOp 					= printProcessStatement;
	opBlk->doIdentifierOp 				= printIdentifier;
	opBlk->doPortModeOp 					= printPortMode;
	opBlk->doDataTypeOp 					= printDataType;
	opBlk->doExpressionOp 				= printExpression;
	opBlk->doEntityDeclCloseOp 		= printClose;
	opBlk->doArchDeclCloseOp 			= printClose;
	opBlk->doPortDeclCloseOp 			= printClose;
	opBlk->doSignalDeclCloseOp 		= printClose;
	opBlk->doVariableDeclCloseOp 		= printClose;
	opBlk->doSignalAssignCloseOp 		= printClose;
	opBlk->doVariableAssignCloseOp 	= printClose;
	opBlk->doProcessCloseOp 			= printClose;
	opBlk->doIfStatementCloseOp		= printClose;
	opBlk->doLoopCloseOp 				= printClose;
	opBlk->doWhileCloseOp 				= printClose;
}


void PrintProgram(struct Program * prog){
	
	// setup block
	struct OperationBlock* opBlk = InitOperationBlock();
	setupDisplayOpBlock(opBlk);
	
	printProg();	
	WalkTree(prog, opBlk);

	// clean up
	free(opBlk);
	printf("\e[0m");
};

