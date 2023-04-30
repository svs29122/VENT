#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include "ast.h"

void PrintUsage(void){
	printf("Usage:\n"
			" tvt -i (interactive menu)\n"
			" tvt adder.vent (perform transpilation)\n"
			" tvt adder.vent --print-tokens\n"
			" tvt adder.vent --print-ast\n"
		);
}

static void printOptions(void){
	printf(" t:   \tTranspile VENT to VHDL\n"
		 	 " r:   \tReverse Transpile VHDL to VENT\n"
			 " h:   \tHelp\n"
			 " q/Q: \tQuit\n"); 
}

static void printWaterfall(void){
	const int waterFallDelay = 250000;

	printf("-----------------------\n");
	printf("The V E N T Transpiler\n");
	usleep(waterFallDelay);
	printf("     H x o e\n");
	usleep(waterFallDelay);
	printf("      D c t r\n");
	usleep(waterFallDelay);
	printf("       L e   r\n");
	usleep(waterFallDelay);
	printf("          p   i\n");
	usleep(waterFallDelay);
	printf("           t   b\n");
	usleep(waterFallDelay);
	printf("                l\n");
	usleep(waterFallDelay);
	printf("                 e\n");
	usleep(waterFallDelay);
	printf("-----------------------\n");
}

void DoMenu(void){

	printWaterfall();

	char opt[256];
	while(1){
		printf("What would you like to do?\n");
		printOptions();
		
		printf("\n=>");
		fgets(opt, sizeof(opt), stdin);
		
		switch(opt[0]){
		   
			case 't':
				printf("Transpiling...\n");
				sleep(3);
				break;
		
			case 'r':
				printf("Reverse transpiling...\n");
				sleep(3);
				break;

			case 'h':
				printf("Help:\n");
				break;
			
			case 'Q':
			case 'q':
				goto exit;
		
			default:
				break;
		}
	}

	exit:
		return;
}

static int ishift;
static bool inProcess;

static char shift(int c){
	printf("\e[0;34m|");
	for(int i=0; i<c-1; i++){
		printf("-");
	}
	printf("\e[0m");
	return ' ';
}

static void printProg(){
	printf("\e[1;32m""Program\r\n");
}

static void printUseStatement(void* stmt){
	printf("\e[1;36m""%cUseStatement: %s\r\n",shift(1), ((struct UseStatement*)stmt)->value);
}

static void printDesignUnit(void* unit){
	printf("\e[1;32m""%cDesignUnit\r\n", shift(1));
}

static void printEntityDecl(void* eDecl){
	printf("\e[0;32m""%cEntityDecl\r\n", shift(2));
	ishift = 3;
}

static void printArchDecl(void* eDecl){
	printf("\e[0;32m""%cArchDecl\r\n", shift(2));
	ishift = 3;
}

static void printIdentifier(void* ident){
	printf("\e[0;35m""%cIdentifier: \'%s\'\r\n", shift(ishift), ((struct Identifier*)ident)->value);
}

static void printPortDecl(void* pDecl){
	printf("\e[0;32m""%cPortDecl\r\n", shift(3));
	ishift = 4;
}

static void printSignalDecl(void* sDecl){
	int shiftVal = 3;
	
	if(inProcess){
		shiftVal++;
	}

	printf("\e[0;32m""%cSignalDecl\r\n", shift(shiftVal));
	ishift = shiftVal+1;
}

static void printVariableDecl(void* vDecl){
	int shiftVal = 3;
	
	if(inProcess){
		shiftVal++;
	}

	printf("\e[0;32m""%cVariableDecl\r\n", shift(shiftVal));
	ishift = shiftVal+1;
}

static void printSignalAssign(void* sAssign){
	int shiftVal = 3;

	if(inProcess){
		shiftVal++;
	}

	printf("\e[0;32m""%cSignalAssign\r\n", shift(shiftVal));
	ishift = shiftVal+1; 
}

static void printProcessStatement(void* proc){
	printf("\e[0;32m""%cProcess\r\n", shift(3));
	ishift = 4;
	
	inProcess = true;
}

static void printProcessClose(void* proc){
	inProcess = false;
}

static void printPortMode(void* pMode){
	printf("\e[0;35m""%cPortMode: \'%s\'\r\n", shift(4), ((struct PortMode*)pMode)->value);
}

static void printDataType(void* dType){
	printf("\e[0;35m""%cDataType: \'%s\'\r\n", shift(ishift), ((struct DataType*)dType)->value);
}

static void printSubExpression(void* expr){
	enum ExpressionType type = ((struct Expression*)expr)->type;

	switch(type) {
	
		case CHAR_EXPR: {
			struct CharExpr* chexp = (struct CharExpr*)expr;
			printf("%s", chexp->literal);
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
	
	printf("\e[0;35m""%cExpression: \'", shift(ishift));
	
	printSubExpression(expr);	

	printf("\'\r\n");
}


void PrintProgram(struct Program * prog){
	
	// setup block
	struct OperationBlock* opBlk = InitOperationBlock();
	opBlk->doUseStatementOp = printUseStatement;	
	opBlk->doDesignUnitOp = printDesignUnit;
	opBlk->doEntityDeclOp = printEntityDecl;
	opBlk->doArchDeclOp = printArchDecl;
	opBlk->doPortDeclOp = printPortDecl;
	opBlk->doSignalDeclOp = printSignalDecl;
	opBlk->doVariableDeclOp = printVariableDecl;
	opBlk->doSignalAssignOp = printSignalAssign;
	opBlk->doProcessOp = printProcessStatement;
	opBlk->doProcessCloseOp = printProcessClose;
	opBlk->doIdentifierOp = printIdentifier;
	opBlk->doPortModeOp = printPortMode;
	opBlk->doDataTypeOp = printDataType;
	opBlk->doExpressionOp = printExpression;
	
	printProg();	
	WalkTree(prog, opBlk);

	// clean up
	free(opBlk);
	printf("\e[0m");
};

