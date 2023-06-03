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

static void printProgram(struct AstNode* prog){
	printf("\e[1;32m""Program\r\n");
}

static void printUseStatement(struct AstNode* stmt){
	indent = 0;
	printf("\e[1;36m""%cUseStatement: %s\r\n",shift(), ((struct UseStatement*)stmt)->value);
}

static void printEntityDecl(struct AstNode* eDecl){
	printf("\e[1;32m""%cEntityDecl\r\n", shift());
	indent++;
}

static void printArchDecl(struct AstNode* aDecl){
	printf("\e[1;32m""%cArchDecl\r\n", shift());
	indent++;
}

static void printIdentifier(struct AstNode* ident){
	printf("\e[0;35m""%cIdentifier: \'%s\'\r\n", shift(), ((struct Identifier*)ident)->value);
}

static void printPortDecl(struct AstNode* pDecl){
	printf("\e[0;32m""%cPortDecl\r\n", shift());
	indent++;
}

static void printSignalDecl(struct AstNode* sDecl){
	printf("\e[0;32m""%cSignalDecl\r\n", shift());
	indent++;
}

static void printVariableDecl(struct AstNode* vDecl){
	printf("\e[0;32m""%cVariableDecl\r\n", shift());
	indent++;
}

static void printForStatement(struct AstNode* fStmt){
	printf("\e[0;33m""%cForStatement\r\n", shift());
	indent++;
}

static void printIfStatement(struct AstNode* ifStmt){
	printf("\e[0;33m""%cIfStatement\r\n", shift());
	indent++;
}

static void printElseClause(struct AstNode* ifStmt){
	printf("\e[0;34m""%c/*Else*/\r\n", shift());
	indent++;
}

static void printLoopStatement(struct AstNode* wStmt){
	printf("\e[0;33m""%cLoopStatement\r\n", shift());
	indent++;
}

static void printSwitchStatement(struct AstNode* swStmt){
	printf("\e[0;33m""%cSwitchStatement\r\n", shift());
	indent++;
}

static void printCaseStatement(struct AstNode* cStmt){
	struct CaseStatement* caseStatement = (struct CaseStatement*)cStmt;

	if(caseStatement->defaultCase){
		printf("\e[0;33m""%cDefaultCase\r\n", shift());
	} else {
		printf("\e[0;33m""%cCaseStatement\r\n", shift());
	}
	indent++;
}

static void printAssertStatement(struct AstNode* nStmt){
	printf("\e[0;33m""%cAssertStatement\r\n", shift());
	indent++;
}

static void printReportStatement(struct AstNode* nStmt){
	printf("\e[0;33m""%cReportStatement\r\n", shift());
	indent++;
}

static void printSeverity(struct AstNode* nStmt){
	struct ReportStatement* rStmt = (struct ReportStatement*)nStmt;
	
	printf("\e[0;35m""%cSeverity: %d\r\n", shift(), rStmt->severity.level);
}

static void printNullStatement(struct AstNode* nStmt){
	printf("\e[0;33m""%cNullStatement\r\n", shift());
}

static void printWhileStatement(struct AstNode* wStmt){
	printf("\e[0;33m""%cWhileStatement\r\n", shift());
	indent++;
}

static void printWaitStatement(struct AstNode* wStmt){
	printf("\e[0;33m""%cWaitStatement\r\n", shift());
}

static void printVariableAssign(struct AstNode* vAssign){
	printf("\e[0;36m""%cVariableAssign\r\n", shift());
	indent++;
}

static void printSignalAssign(struct AstNode* sAssign){
	printf("\e[0;36m""%cSignalAssign\r\n", shift());
	indent++;
}

static void printProcessStatement(struct AstNode* proc){
	indent = 2;
	printf("\e[0;33m""%cProcess\r\n", shift());
	indent++;
}

static void printPortMode(struct AstNode* pMode){
	printf("\e[0;35m""%cPortMode: \'%s\'\r\n", shift(), ((struct PortMode*)pMode)->value);
}

static void printDataType(struct AstNode* dType){
	printf("\e[0;35m""%cDataType: \'%s\'\r\n", shift(), ((struct DataType*)dType)->value);
}

static void printAssignmentOp(struct AstNode* vAssign){
	char* op = ((struct VariableAssign*)vAssign)->op;
	printf("\e[0;35m""%cOperator:   \'%s\'\r\n", shift(), (char*)op);
}

static void printSubExpression(struct Expression* expr){
	enum ExpressionType type = expr->type;

	switch(type) {
	
		case CHAR_EXPR: {
			struct CharExpr* chexp = (struct CharExpr*)expr;
			printf("%s", chexp->literal);
			break;
		}

		case STRING_EXPR: {
			struct StringExpr* stexp = (struct StringExpr*)expr;
			printf("%s", stexp->literal);
			break;
		}

		case NUM_EXPR: {
			struct NumExpr* nexp = (struct NumExpr*)expr;
			printf("%s", nexp->literal);
			break;
		}

		case BINARY_EXPR:{
			struct BinaryExpr* bexp = (struct BinaryExpr*) expr;
			printSubExpression(bexp->left);
			printf(" %s ", bexp->op);
			printSubExpression(bexp->right);
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

static void printRange(struct AstNode* rng){
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
static void printExpression(struct Expression* expr){
	printf("\e[0;35m""%cExpression: \'", shift());
	
	printSubExpression(expr);	
	printf("\'\r\n");
}

static void printSpecial(struct AstNode* node){

	switch(node->type){

		case AST_VASSIGN:
			printAssignmentOp(node);
			break;

		case AST_IF:
			printElseClause(node);
			break;
	
		case AST_REPORT:
			printSeverity(node);
			break;

		default:
			break;
	}
}

static void printClose(struct AstNode* none){
	indent--;
}

static void printDefault(struct AstNode* node){

	switch(node->type){
		
		case AST_PROGRAM:
			printProgram(node);
			break;

		case AST_USE:
			printUseStatement(node);
			break;
		
		case AST_ENTITY:
			printEntityDecl(node);
			break;

		case AST_ARCHITECTURE:
			printArchDecl(node);
			break;

		case AST_PORT:
			printPortDecl(node);
			break;

		case AST_PROCESS:
			printProcessStatement(node);
			break;

		case AST_FOR:
			printForStatement(node);
			break;

		case AST_IF:
		case AST_ELSIF:
			printIfStatement(node);
			break;

		case AST_LOOP:
			printLoopStatement(node);
			break;

		case AST_ASSERT:
			printAssertStatement(node);
			break;

		case AST_REPORT:
			printReportStatement(node);
			break;

		case AST_NULL:
			printNullStatement(node);
			break;

		case AST_SWITCH:
			printSwitchStatement(node);
			break;

		case AST_CASE:
			printCaseStatement(node);
			break;

		case AST_WAIT:
			printWaitStatement(node);
			break;

		case AST_WHILE:
			printWhileStatement(node);
			break;

		case AST_SASSIGN:
			printSignalAssign(node);
			break;

		case AST_VASSIGN:
			printVariableAssign(node);
			break;

		case AST_SDECL:
			printSignalDecl(node);
			break;

		case AST_VDECL:
			printVariableDecl(node);
			break;

		case AST_IDENTIFIER:
			printIdentifier(node);
			break;

		case AST_PMODE:
			printPortMode(node);
			break;

		case AST_DTYPE:
			printDataType(node);
			break;

		case AST_RANGE:
			printRange(node);
			break;

		default:
			break;
	}
}

void PrintProgram(struct Program* prog){
	
	// setup block
	struct OperationBlock opBlk = {
		.doDefaultOp 		= printDefault,
		.doCloseOp 			= printClose,
		.doSpecialOp 		= printSpecial,
		.doExpressionOp 	= printExpression,
	};
	
	WalkTree(prog, &opBlk);

	// clean up
	printf("\e[0m");
};

