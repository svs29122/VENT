#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "ast.h"

void PrintUsage(void){
	printf("Usage:\n"
			" tvt -i (interactive menu)\n"
			" tvt -t adder.vent (perform transpilation)\n"
			" tvt -r adder.vhdl (perform reverse transpilation\n"
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
	printf("\e[1;36m""%cUseStatement: %s\r\n",shift(1), ((UseStatement*)stmt)->value);
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
	printf("\e[0;35m""%cIdentifier: \'%s\'\r\n", shift(ishift), ((Identifier*)ident)->value);
}

static void printPortDecl(void* pDecl){
	printf("\e[0;32m""%cPortDecl\r\n", shift(3));
	ishift = 4;
}

static void printSignalDecl(void* sDecl){
	printf("\e[0;32m""%cSignalDecl\r\n", shift(3));
	ishift = 4;
}

static void printPortMode(void* pMode){
	printf("\e[0;35m""%cPortMode: \'%s\'\r\n", shift(4), ((PortMode*)pMode)->value);
}

static void printDataType(void* dType){
	printf("\e[0;35m""%cDataType: \'%s\'\r\n", shift(4), ((DataType*)dType)->value);
}

void PrintProgram(Program * prog){
	
	// setup block
	OperationBlock* opBlk = initOperationBlock();
	opBlk->doUseStatementOp = printUseStatement;	
	opBlk->doDesignUnitOp = printDesignUnit;
	opBlk->doEntityDeclOp = printEntityDecl;
	opBlk->doArchDeclOp = printArchDecl;
	opBlk->doPortDeclOp = printPortDecl;
	opBlk->doSignalDeclOp = printSignalDecl;
	opBlk->doIdentifierOp = printIdentifier;
	opBlk->doPortModeOp = printPortMode;
	opBlk->doDataTypeOp = printDataType;
	
	printProg();	
	WalkTree(prog, opBlk);

	// clean up
	free(opBlk);
	printf("\e[0m");
};

