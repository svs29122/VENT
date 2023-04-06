#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

FILE* vhdlFile;

static void emitUseStatement(void* stmt){
	struct UseStatement* useStmt = (struct UseStatement*)stmt;
	
	int libCnt = -1;
	char currChar;
	while(currChar != '.'){
		libCnt++;
		currChar = useStmt->value[libCnt];
	}

	//TODO: Need to figure out what to do when multiple use statements for same library	
	char* library = malloc(sizeof(char) * libCnt + 1);
	memcpy(library, useStmt->value, libCnt);
	library[libCnt] = '\0';
	fprintf(vhdlFile, "library  %s;\n", library);
	
	fprintf(vhdlFile, "use %s;\n", useStmt->value);
}



void TranspileProgram(struct Program* prog, char* fileName){

	//setup block
	struct OperationBlock* op = InitOperationBlock();
	op->doUseStatementOp = emitUseStatement;
	
	//TODO: fix this so we can grab the existing filename, but remove the .vent extension
	fileName = NULL;	
	
	if(fileName == NULL) {
		vhdlFile = fopen("./a.vhdl", "w");
	}

	WalkTree(prog, op);

	fprintf(vhdlFile,"\n");

	fclose(vhdlFile);
	free(op);
}
