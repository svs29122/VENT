#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ast.h>

static FILE* vhdlFile;

struct expressionStatus {
	bool incoming;
	bool close;
	char assignmentOp[4];
} static eStat = {false, false, 0};

static int indent = 1;
static char emitIndent(){
	for(int i=1; i<indent; i++){
		fprintf(vhdlFile, "\t");
	}
	return '\t';
}

static void emitUseStatement(struct AstNode* stmt){
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

static void emitEntityDeclaration(struct AstNode* edecl){
	struct EntityDecl* entDecl = (struct EntityDecl*)edecl;
	char* entIdent = entDecl->name->value;

	fprintf(vhdlFile, "\nentity %s is\n", entIdent);
}

static void emitEntityDeclarationClose(struct AstNode* edecl){
	struct EntityDecl* entDecl = (struct EntityDecl*)edecl;
	char* entIdent = entDecl->name->value;

	//overwrite that last semicolon
	fseek(vhdlFile, -2, SEEK_CUR);
	fprintf(vhdlFile, "\n\t);\nend %s;\n\n", entIdent);
}

static void emitPortDeclarationOpen(struct AstNode* eDecl){
	struct EntityDecl* entDecl = (struct EntityDecl*)eDecl;
	
	if(entDecl->ports != NULL){
		fprintf(vhdlFile, "%cport(\n", emitIndent());
	}	
}

static void emitPortDeclaration(struct AstNode* pdecl){
	struct PortDecl* portDecl = (struct PortDecl*) pdecl;
	fprintf(vhdlFile, "\t\t%s: ", portDecl->name->value);
}

static void emitPortMode(struct AstNode* pmode){
	struct PortMode* portMode = (struct PortMode*) pmode;

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

static void emitArchitectureDeclaration(struct AstNode* aDecl){
	struct ArchitectureDecl* archDecl = (struct ArchitectureDecl*) aDecl;
	
	char* archName = archDecl->archName->value;
	char* entName = archDecl->entName->value;
	
	fprintf(vhdlFile, "architecture %s of %s is\n", archName, entName);
}

static void emitArchitectureDeclarationOpen(struct AstNode* aDecl){
	fprintf(vhdlFile, "begin\n\n");
}

static void emitArchitectureDeclarationClose(struct AstNode* aDecl){
	struct ArchitectureDecl* archDecl = (struct ArchitectureDecl*) aDecl;

	char* archName = archDecl->archName->value;
	
	fprintf(vhdlFile, "\nend architecture %s;\n", archName);
}

static void emitProcess(struct AstNode* proc){
	fprintf(vhdlFile, "\n\tprocess is \n"); 
	indent++;
}

static void emitProcessOpen(struct AstNode* proc){
	fprintf(vhdlFile, "\tbegin\n\n");
}

static void emitProcessClose(struct AstNode* proc){
	indent--;
	fprintf(vhdlFile, "\tend process;\n");
}

static void emitWhileLoop(struct AstNode* wstmt){

	struct WhileStatement* whileStat = (struct WhileStatement*)wstmt;	
	fprintf(vhdlFile, "%cwhile", emitIndent());
	indent++;

	if(whileStat->condition){
		eStat.incoming = true;
		memcpy(eStat.assignmentOp, "\0", 1);
	}
}

static void emitWhileLoopOpen(struct AstNode* wstmt){
	fprintf(vhdlFile, " loop\n");
}

static void emitWhileLoopClose(struct AstNode* wstmt){
	indent--;
	fprintf(vhdlFile, "%cend loop;\n\n", emitIndent());
}


static void emitSignalDeclaration(struct AstNode* sDecl){
	struct SignalDecl* sigDecl = (struct SignalDecl*) sDecl;

	char* sigName = sigDecl->name->value;
	fprintf(vhdlFile, "%csignal %s: ", emitIndent(), sigName);		

	if(sigDecl->expression){
		eStat.incoming = true;
		eStat.close = true;
		memcpy(eStat.assignmentOp, " :=", 4);
	} 
}

static void emitVariableDeclaration(struct AstNode* vDecl){
	struct VariableDecl* varDecl = (struct VariableDecl*) vDecl;

	char* varName = varDecl->name->value;
	fprintf(vhdlFile, "%cvariable %s: ", emitIndent(), varName);		

	if(varDecl->expression){
		eStat.incoming = true;
		eStat.close = true;
		memcpy(eStat.assignmentOp, " :=", 4);
	}
}

static void emitSignalAssignment(struct AstNode* sAssign){
	struct SignalAssign* sigAssign = (struct SignalAssign*) sAssign;

	char* target = sigAssign->target->value;
	fprintf(vhdlFile, "%c%s ", emitIndent(), target);		

	if(sigAssign->expression){
		eStat.incoming = true;
		eStat.close = true;
		memcpy(eStat.assignmentOp, "<=", 3);
	}
}

static void emitVariableAssignment(struct AstNode* vAssign){
	struct VariableAssign* varAssign = (struct VariableAssign*) vAssign;

	char* target = varAssign->target->value;
	fprintf(vhdlFile, "%c%s ", emitIndent(), target);		

	if(varAssign->expression){
		eStat.incoming = true;
		eStat.close = true;
		memcpy(eStat.assignmentOp, ":=", 3);
	}
}

static void emitDataType(struct AstNode* dtype){
	struct DataType* dataType = (struct DataType*) dtype;
	
	char* typeName = dataType->value;
	if(strcmp(typeName, "stl") == 0){
		fprintf(vhdlFile, "std_logic");
	} else if(strcmp(typeName, "stlv") == 0){
		fprintf(vhdlFile, "std_logic_vector");
	} else if(strcmp(typeName, "int") == 0){
		fprintf(vhdlFile, "integer");
	}

	if(!eStat.incoming) {
		fprintf(vhdlFile, ";\n");
	}
}

static void emitSubExpression(struct Expression* expr){
	enum ExpressionType type = expr->type;

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
          emitSubExpression(bexp->left);
          fprintf(vhdlFile, " %s ", bexp->op);
          emitSubExpression(bexp->right);
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

static void emitExpression(struct Expression* expr){
	
	fprintf(vhdlFile, "%s ", eStat.assignmentOp);	
	emitSubExpression(expr);
	if(eStat.close){
		fprintf(vhdlFile, ";\n");
	}

	memset(&eStat, 0, sizeof(struct expressionStatus));
}

static void emitClose(struct AstNode* node){

	switch(node->type){

		case AST_ENTITY:
			emitEntityDeclarationClose(node);
			break;
		
		case AST_ARCHITECTURE:
			emitArchitectureDeclarationClose(node);
			break;
		
		case AST_PROCESS:
			emitProcessClose(node);
			break;
		
		case AST_WHILE:
			emitWhileLoopClose(node);
			break;
		
		default:
			break;
	}
}

static void emitOpen(struct AstNode* node){

	switch(node->type){
		
		case AST_ENTITY:
			emitPortDeclarationOpen(node);
			break;
		
		case AST_ARCHITECTURE:
			emitArchitectureDeclarationOpen(node);
			break;
		
		case AST_PROCESS:
			emitProcessOpen(node);
			break;
		
		case AST_WHILE:
			emitWhileLoopOpen(node);
			break;
		
		default:
			break;
	}
}

static void emitDefault(struct AstNode* node){

	switch(node->type){
		
      case AST_USE:
         emitUseStatement(node);
         break;

      case AST_ENTITY:
         emitEntityDeclaration(node);
         break;

      case AST_ARCHITECTURE:
         emitArchitectureDeclaration(node);
         break;

      case AST_PORT:
         emitPortDeclaration(node);
         break;

      case AST_PROCESS:
			emitProcess(node);
         break;

      case AST_FOR:
         break;

      case AST_ELSIF:
         break;

      case AST_LOOP:
         break;

      case AST_WAIT:
         break;

    	case AST_WHILE:
			emitWhileLoop(node);
         break;

      case AST_SASSIGN:
         emitSignalAssignment(node);
         break;

      case AST_VASSIGN:
         emitVariableAssignment(node);
         break;

      case AST_SDECL:
         emitSignalDeclaration(node);
         break;

      case AST_VDECL:
         emitVariableDeclaration(node);
         break;

      case AST_IDENTIFIER:
         break;

      case AST_PMODE:
         emitPortMode(node);
         break;

      case AST_DTYPE:
         emitDataType(node);
         break;

      case AST_RANGE:
         break;

      default:
         break;
	}
}

void TranspileProgram(struct Program* prog, char* fileName){

	//setup block
	struct OperationBlock opBlk = {
		.doDefaultOp		= emitDefault,
		.doOpenOp			= emitOpen,
		.doCloseOp			= emitClose,
		.doExpressionOp	= emitExpression,
	};
	
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

		WalkTree(prog, &opBlk);

		fclose(vhdlFile);
	}
}
