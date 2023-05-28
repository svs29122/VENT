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

static void printProgram(){
	printf("\e[1;32m""Program\r\n");
}

static void printUseStatement(void* stmt){
	indent = 0;
	printf("\e[1;36m""%cUseStatement: %s\r\n",shift(), ((struct UseStatement*)stmt)->value);
}

static void printEntityDecl(void* eDecl){
	printf("\e[1;32m""%cEntityDecl\r\n", shift());
	indent++;
}

static void printArchDecl(void* aDecl){
	printf("\e[1;32m""%cArchDecl\r\n", shift());
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

static void printForStatement(void* fStmt){
	printf("\e[0;33m""%cForStatement\r\n", shift());
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

static void printAssignmentOp(void* vAssign){
	char* op = (struct VariableAssign*)vAssign)->op;
	printf("\e[0;35m""%cOperator:   \'%s\'\r\n", shift(), (char*)op);
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

static void printRange(void* rng){
	printf("\e[0;35m""%cRange: ", shift());

	struct Range* range = (struct Range*)rng;
	printSubExpression(range->left);
	
	if(range->right){
		if(range->descending){
			printf(" downto ");
		} else {
			printf(" to ");
		}

		printSubExpression(range->right);
	}
	printf("\r\n");
}


// Operation Block ops
static void printExpression(void* expr){
	printf("\e[0;35m""%cExpression: \'", shift());
	
	printSubExpression(expr);	
	printf("\'\r\n");
}

static void printSpecial(struct AstNode* node){

	switch(node->type){

		case AST_VASSIGN:
			printAssignmentOp((void*)node);
			break;

		case AST_IF:
			printElseClause((void*)node);
			break;

		default:
			printf("Unknown node type in SpecialOp...\e[0m\r\n");
			break;
			
	}
}

static void printClose(struct AstNode* none){
	indent--;
}

static void printDefault(struct AstNode* node){

	switch(node->type){
		
		case AST_PROGRAM:
			printProgram((void*)node);
			break;

		case AST_USE:
			printUseStatement((void*)node);
			break;
		
		case AST_ENTITY:
			printEntityDecl((void*)node);
			break;

		case AST_ARCHITECTURE:
			printArchDecl((void*)node);
			break;

		case AST_PORT:
			printPortDecl((void*)node);
			break;

		case AST_PROCESS:
			printProcessStatement((void*)node);
			break;

		case AST_FOR:
			printForStatement((void*)node);
			break;

		case AST_IF:
			printIfStatement((void*)node);
			break;

		case AST_LOOP:
			printLoopStatement((void*)node);
			break;

		case AST_WAIT:
			printWaitStatement((void*)node);
			break;

		case AST_WHILE:
			printWhileStatement((void*)node);
			break;

		case AST_SASSIGN:
			printSignalAssign((void*)node);
			break;

		case AST_VASSIGN:
			printVariableAssign((void*)node);
			break;

		case AST_SDECL:
			printSignalDecl((void*)node);
			break;

		case AST_VDECL:
			printVariableDecl((void*)node);
			break;

		case AST_IDENTIFIER:
			printIdentifier((void*)node);
			break;

		case AST_PMODE:
			printPortMode((void*)node);
			break;

		case AST_DTYPE:
			printDataType((void*)node);
			break;

		case AST_RANGE:
			printRange((void*)node);
			break;

		default:
			printf("Unknown node type in DefaultOp...\e[0m\r\n");
			break;

	}
}

void PrintProgram(struct Program * prog){
	
	// setup block
	struct OperationBlock* opBlk 		= InitOperationBlock();
	opBlk->doDefaultOp					= printDefault;
	opBlk->doCloseOp						= printClose;
	opBlk->doSpecialOp					= printSpecial;
	opBlk->doExpressionOp 				= printExpression;
	
	WalkTree(prog, opBlk);

	// clean up
	free(opBlk);
	printf("\e[0m");
};

