#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ast.h>

static FILE* vhdlFile;

static bool incomingExpression = false;
static bool closeExpression = false;
static char assignmentOp[4];

static void emitUseStatement(void* stmt){
	struct UseStatement* useStmt = (struct UseStatement*)stmt;
	
	int libCnt = -1;
	char currChar = 0;
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
		closeExpression = true;
		memcpy(assignmentOp, " :=", 4);
	}

}

static void emitVariableDeclaration(void* vDecl){
	struct VariableDecl* varDecl = (struct VariableDecl*) vDecl;

	char* varName = varDecl->name->value;
	fprintf(vhdlFile, "\t\tvariable %s: ", varName);		

	if(varDecl->expression){
		incomingExpression = true;
		closeExpression = true;
		memcpy(assignmentOp, " :=", 4);
	}
}

static void emitSignalAssignment(void* sAssign){
	struct SignalAssign* sigAssign = (struct SignalAssign*) sAssign;

	char* target = sigAssign->target->value;
	fprintf(vhdlFile, "\t%s ", target);		

	if(sigAssign->expression){
		incomingExpression = true;
		closeExpression = true;
		memcpy(assignmentOp, "<=", 3);
	}
}

static void emitVariableAssignment(void* vAssign){
	struct VariableAssign* varAssign = (struct VariableAssign*) vAssign;

	char* target = varAssign->target->value;
	fprintf(vhdlFile, "\t\t\t%s ", target);		

	if(varAssign->expression){
		incomingExpression = true;
		closeExpression = true;
		memcpy(assignmentOp, ":=", 3);
	}
}

static void emitDataType(void* dtype){
	struct DataType* dataType = (struct DataType*) dtype;
	
	char* typeName = dataType->value;
	if(strcmp(typeName, "stl") == 0){
		fprintf(vhdlFile, "std_logic");
	} else if(strcmp(typeName, "stlv") == 0){
		fprintf(vhdlFile, "std_logic_vector");
	} else if(strcmp(typeName, "int") == 0){
		fprintf(vhdlFile, "integer");
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

		case NUM_EXPR: {
			struct NumExpr* nexp = (struct NumExpr*)expr;
			fprintf(vhdlFile, "%s", nexp->literal);
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
	if(closeExpression){
		fprintf(vhdlFile, ";\n");
	}

	incomingExpression = false;
	closeExpression = false;
}

static void emitProcess(void* proc){
	
	fprintf(vhdlFile, "\tprocess is \n"); 
}

static void emitProcessOpen(void* proc){
	
	fprintf(vhdlFile, "\tbegin\n\n");
}

static void emitProcessClose(void* proc){
	
	fprintf(vhdlFile, "\n\tend process;\n");
}

static void emitWhileLoop(void* wstmt){

	struct WhileStatement* whileStat = (struct WhileStatement*)wstmt;	
	fprintf(vhdlFile, "\t\twhile");

	if(whileStat->condition){
		incomingExpression = true;
		memcpy(assignmentOp, "\0", 1);
	}
}

static void emitWhileLoopOpen(void* wstmt){

	fprintf(vhdlFile, " loop\n");
}

static void emitWhileLoopClose(void* wstmt){
	
	fprintf(vhdlFile, "\t\tend loop;\n");
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
	op->doVariableDeclOp = emitVariableDeclaration;
	op->doSignalAssignOp = emitSignalAssignment;
	op->doVariableAssignOp = emitVariableAssignment;
	op->doDataTypeOp = emitDataType;
	op->doExpressionOp = emitExpression;
	op->doProcessOp = emitProcess;	
	op->doProcessOpenOp = emitProcessOpen;	
	op->doProcessCloseOp = emitProcessClose;	
	op->doWhileStatementOp = emitWhileLoop;
	op->doWhileOpenOp = emitWhileLoopOpen;
	op->doWhileCloseOp = emitWhileLoopClose;
	
	//setup filename
	if(fileName != NULL){
		char* prevTok = NULL;
		char* currTok = NULL;
		char* nextTok = NULL;

		nextTok  = strtok(fileName, "./");
		while(nextTok != NULL){
			prevTok = currTok;
			currTok = nextTok;
			nextTok = strtok(NULL, "./");
		}

		if(prevTok != NULL && strcmp(currTok, "vent") == 0){
			strcat(prevTok, ".vhdl");
			vhdlFile = fopen(prevTok, "w");
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
