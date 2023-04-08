#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ast.h"

static FILE* vhdlFile;

static bool incomingExpression = false;
static char assignmentOp[4];

static void emitUseStatement(void* stmt){
	struct UseStatement* useStmt = (struct UseStatement*)stmt;
	
	int libCnt = -1;
	char currChar;
	while(currChar != '.'){
		libCnt++;
		currChar = useStmt->value[libCnt];
	}

	//TODO: Need to figure out what to do when multiple use statements for same library	
	// consider storing the library in the UseStatment AST node! 
	char* library = malloc(sizeof(char) * libCnt + 1);
	memcpy(library, useStmt->value, libCnt);
	library[libCnt] = '\0';
	fprintf(vhdlFile, "library %s;\n", library);
	
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

static void emitPortDeclaration(void* pdecl){
	struct PortDecl* portDecl = (struct PortDecl*) pdecl;

	fprintf(vhdlFile, "\t\t%s: ", portDecl->name->value);

	//if(port->decl->expression) incomingExpression = true;	
}

static void emitPortMode(void* pmode){
	struct PortMode* portMode = (struct PortMode*) pmode;

	//TODO: consider using TokenType instead of nasty strcmp

	char* pVal = portMode->value;
	if(strcmp(pVal, "->") == 0){
		fprintf(vhdlFile, "in ");
	} else if(strcmp(pVal, "<-") == 0){
		fprintf(vhdlFile, "out ");
	} else if(strcmp(pVal, "<->") == 0){
		fprintf(vhdlFile, "inout ");
	} else if(strcmp(pVal, ">-<") == 0){
		fprintf(vhdlFile, "buffer ");
	}

}

static void emitPortDeclarationClose(void* ports){
	//overwrite that last semicolon
	fseek(vhdlFile, -2, SEEK_CUR);

	if(ports != NULL){
		fprintf(vhdlFile, "\n\t);\n");
	}	
}

static void emitArchitectureDeclaration(void* aDecl){
	struct ArchitectureDecl* archDecl = (struct ArchitectureDecl*) aDecl;
	
	char* archName = archDecl->archName->value;
	char* entName = archDecl->entName->value;
	
	fprintf(vhdlFile, "architecture %s of %s is\n\n", archName, entName);
}

static void emitArchitectureDeclarationOpen(void* aDecl){
	fprintf(vhdlFile, "\nbegin\n\n");
}

static void emitArchitectureDeclarationClose(void* aDecl){
	struct ArchitectureDecl* archDecl = (struct ArchitectureDecl*) aDecl;

	char* archName = archDecl->archName->value;
	
	fprintf(vhdlFile, "\nend architecture %s;\n", archName);
}

static void emitSignalDeclaration(void* sDecl){
	struct SignalDecl* sigDecl = (struct SignalDecl*) sDecl;

	char* sigName = sigDecl->name->value;
	fprintf(vhdlFile, "\tsignal %s: ", sigName);		

	if(sigDecl->expression){
		incomingExpression = true;
		memcpy(assignmentOp, " :=", 4);
	}

}

static void emitSignalAssignment(void* sAssign){
	struct SignalAssign* sigAssign = (struct SignalAssign*) sAssign;

	char* target = sigAssign->target->value;
	fprintf(vhdlFile, "\t%s ", target);		

	if(sigAssign->expression){
		incomingExpression = true;
		memcpy(assignmentOp, "<=", 3);
	}
}

static void emitDataType(void* dtype){
	struct DataType* dataType = (struct DataType*) dtype;
	
	char* typeName = dataType->value;
	if(strcmp(typeName, "stl") == 0){
		fprintf(vhdlFile, "std_logic");
	} else if(strcmp(typeName, "stlv") == 0){
		fprintf(vhdlFile, "std_logic_vector");
	}

	if(!incomingExpression) {
		fprintf(vhdlFile, ";\n");
	}
}

static void emitSubExpression(void* expr){
	enum ExpressionType type = ((struct Expression*)expr)->type;

	switch(type){
		
		case CHAR_EXPR: {
			struct CharExpr* chexp = (struct CharExpr*)expr;
			fprintf(vhdlFile, "'%s'", chexp->literal);
			break;
		}

		case BINARY_EXPR:{
          struct BinaryExpr* bexp = (struct BinaryExpr*) expr;
          emitSubExpression((void*)bexp->left);
          fprintf(vhdlFile, " %s ", bexp->op);
          emitSubExpression((void*)bexp->right);
          break;
       }
 
       case NAME_EXPR: {
          //NameExpr* nexp = (NameExpr*) expr;
          //printf("\e[0;35m""\'%s\'\r\n", nexp->name->value);
          struct Identifier* ident = (struct Identifier*)expr;
          fprintf(vhdlFile, "%s", ident->value);
          break;
       }
 
       default:
          break;
	}	
}

static void emitExpression(void* expr){
	
	fprintf(vhdlFile, "%s ", assignmentOp);	
	emitSubExpression(expr);
	fprintf(vhdlFile, ";\n");

	incomingExpression = false;
}

void TranspileProgram(struct Program* prog, char* fileName){

	//setup block
	struct OperationBlock* op = InitOperationBlock();
	op->doUseStatementOp = emitUseStatement;
	op->doEntityDeclOp = emitEntityDeclaration;
	op->doEntityDeclCloseOp = emitEntityDeclarationClose;
	op->doPortDeclOpenOp = emitPortDeclarationOpen;
	op->doPortDeclOp = emitPortDeclaration;
	op->doPortModeOp = emitPortMode;
	op->doPortDeclCloseOp = emitPortDeclarationClose;
	op->doArchDeclOp = emitArchitectureDeclaration;
	op->doArchDeclOpenOp = emitArchitectureDeclarationOpen;
	op->doArchDeclCloseOp = emitArchitectureDeclarationClose;
	op->doSignalDeclOp = emitSignalDeclaration;
	op->doSignalAssignOp = emitSignalAssignment;
	op->doDataTypeOp = emitDataType;
	op->doExpressionOp = emitExpression;
	
	//setup filename
	char* newName = NULL;
	if(fileName != NULL){
		newName = strtok(fileName, ".");
		if(newName != NULL){
			strcat(newName, ".vhdl");
			vhdlFile = fopen(newName, "w");
		} else {
			vhdlFile = fopen("./a.vhdl", "w");
		}
	} else {
		vhdlFile = fopen("./a.vhdl", "w");
	}

	//let's transpile this baby
	if(vhdlFile != NULL){

		fprintf(vhdlFile,"--\n-- This file was produced using TVT (The VENT Transpiler)\n--\n\n");

		WalkTree(prog, op);

		fclose(vhdlFile);
	}

	free(op);
}
