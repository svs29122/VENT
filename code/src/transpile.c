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
	
	free(library);
}

static void emitEntityDeclaration(void* edecl){
	struct EntityDecl* entDecl = (struct EntityDecl*)edecl;
	char* entIdent = entDecl->name->value;

	fprintf(vhdlFile, "\nentity %s is\n", entIdent);
}

static void emitEntityDeclarationClose(void* edecl){
	struct EntityDecl* entDecl = (struct EntityDecl*)edecl;
	char* entIdent = entDecl->name->value;

	fprintf(vhdlFile, "end %s;\n\n", entIdent);
}

static void emitPortDeclarationOpen(void* ports){
	if(ports != NULL){
		fprintf(vhdlFile, "\tport(\n");
	}	
}

static void emitPortDeclarationClose(void* ports){
	//TODO: may need to fseek back and remove last semicolon from last portdecl

	if(ports != NULL){
		fprintf(vhdlFile, "\t);\n");
	}	
}

static void emitPortDeclaration(void* pdecl){
	struct PortDecl* portDecl = (struct PortDecl*) pdecl;

	fprintf(vhdlFile, "\t\t%s: ", portDecl->name->value);

	//TODO: consider using TokenType instead of nasty strcmp

	char* portMode = portDecl->pmode->value;
	if(strcmp(portMode, "->") == 0){
		fprintf(vhdlFile, "in ");
	} else if(strcmp(portMode, "<-") == 0){
		fprintf(vhdlFile, "out ");
	} else if(strcmp(portMode, "<->") == 0){
		fprintf(vhdlFile, "inout ");
	} else if(strcmp(portMode, ">-<") == 0){
		fprintf(vhdlFile, "buffer ");
	}

	char* dataType = portDecl->dtype->value;
	if(strcmp(dataType, "stl") == 0){
		fprintf(vhdlFile, "std_logic;\n");
	} else if(strcmp(dataType, "stlv") == 0){
		fprintf(vhdlFile, "std_logic_vector;\n");
	}
}

void TranspileProgram(struct Program* prog, char* fileName){

	//setup block
	struct OperationBlock* op = InitOperationBlock();
	op->doUseStatementOp = emitUseStatement;
	op->doEntityDeclOp = emitEntityDeclaration;
	op->doEntityDeclCloseOp = emitEntityDeclarationClose;
	op->doPortDeclOpenOp = emitPortDeclarationOpen;
	op->doPortDeclCloseOp = emitPortDeclarationClose;
	op->doPortDeclOp = emitPortDeclaration;
	
	//TODO: fix this so we can grab the existing filename, but remove the .vent extension
	fileName = NULL;	
	
	if(fileName == NULL) {
		vhdlFile = fopen("./a.vhdl", "w");
	}

	if(vhdlFile != NULL){

		fprintf(vhdlFile,"--\n-- This file was produced using TVT (The VENT Transpiler)\n--\n\n");

		WalkTree(prog, op);

		fprintf(vhdlFile,"\n");
		fclose(vhdlFile);
	}

	free(op);
}
