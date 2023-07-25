#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <ast.h>

static FILE* vhdlFile;

struct expressionStatus {
	bool incoming;
	bool close;
	bool ignore;
	bool list;
	char assignmentOp[4];
} static eStat = {false, false, false, false, 0};

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

static void emitGenericDeclarationOpen(struct AstNode* gDecl){
	fprintf(vhdlFile, "%cgeneric(\n", emitIndent());
}

static void emitGenericDeclarationSpecial(struct AstNode* gDecl){
	//overwrite that last semicolon
	fseek(vhdlFile, -2, SEEK_CUR);
	fprintf(vhdlFile, "\n\t);\n");
}

static void emitGenericDeclaration(struct AstNode* gdecl){
	struct GenericDecl* genericDecl = (struct GenericDecl*) gdecl;
	fprintf(vhdlFile, "\t\t%s: ", genericDecl->name->value);
}

static void emitPortDeclarationOpen(struct AstNode* pDecl){
	fprintf(vhdlFile, "%cport(\n", emitIndent());
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
	fprintf(vhdlFile, "begin\n");
}

static void emitArchitectureDeclarationClose(struct AstNode* aDecl){
	struct ArchitectureDecl* archDecl = (struct ArchitectureDecl*) aDecl;

	char* archName = archDecl->archName->value;
	
	fprintf(vhdlFile, "\nend architecture %s;\n\n", archName);
}

static void emitProcess(struct AstNode* proc){
	fprintf(vhdlFile, "\n\tprocess is \n"); 
	indent++;
}

static void emitProcessOpen(struct AstNode* proc){
	fprintf(vhdlFile, "\tbegin\n");
}

static void emitProcessClose(struct AstNode* proc){
	indent--;
	fprintf(vhdlFile, "\tend process;\n");
}

static void emitIfStatement(struct AstNode* ifstmt){
	
	struct IfStatement* ifStatement = (struct IfStatement*)ifstmt;
	bool inAnElsIf = ifStatement->inElsIf;

	if(inAnElsIf) indent--; 
	fprintf(vhdlFile,"%c", emitIndent());

	if(inAnElsIf) fprintf(vhdlFile,"els");
	fprintf(vhdlFile, "if");
	indent++;
}

static void emitIfOpen(struct AstNode* ifstmt){
	fprintf(vhdlFile, " then\n");
}

static void emitIfClose(struct AstNode* ifstmt){
	indent--;
	fprintf(vhdlFile, "%cend if;\n", emitIndent());
}

static void emitElse(struct AstNode* efstmt){
	indent--;
	fprintf(vhdlFile, "%celse\n", emitIndent());
	indent++;
}

static void emitCaseStatement(struct AstNode* sstmt){
	fprintf(vhdlFile, "%ccase", emitIndent());
	indent++;
}

static void emitCaseOpen(struct AstNode* sstmt){
	fprintf(vhdlFile, " is\n");
}

static void emitCaseClose(struct AstNode* sstmt){
	indent--;
	fprintf(vhdlFile, "%cend case;\n", emitIndent());
}

static void emitWhenStatement(struct AstNode* cstmt){
	struct CaseStatement* caseStmt = (struct CaseStatement*)cstmt;	

	fprintf(vhdlFile, "%cwhen", emitIndent());
	
	if(caseStmt->defaultCase) fprintf(vhdlFile, " others");
	
	indent++;
}

static void emitWhenOpen(struct AstNode* cstmt){
	fprintf(vhdlFile, " =>\n");
}

static void emitWhenClose(struct AstNode* cstmt){
	indent--;
}

static void emitWhenSpecial(struct AstNode* cstmt){
	fprintf(vhdlFile, " |");
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

static void emitForLoop(struct AstNode* fstmt){
	struct ForStatement* forStat = (struct ForStatement*)fstmt;	

	fprintf(vhdlFile, "%cfor %s in", emitIndent(), forStat->parameter->value);
	indent++;
}

static void emitLoopOpen(struct AstNode* wstmt){
	fprintf(vhdlFile, " loop\n");
}

static void emitLoopClose(struct AstNode* wstmt){
	indent--;
	fprintf(vhdlFile, "%cend loop;\n\n", emitIndent());
}

static void emitLoop(struct AstNode* lstmt){
	fprintf(vhdlFile, "%cloop\n", emitIndent());
	indent++;
}

static void emitWait(struct AstNode* wstmt){
	fprintf(vhdlFile, "%cwait;\n", emitIndent());
}

//TODO: may need to do Asserts and Reports together
static void emitAssert(struct AstNode* astmt){
	struct AssertStatement* aStat = (struct AssertStatement*)astmt;

	fprintf(vhdlFile, "%cassert", emitIndent());
	//fprintf(vhdlFile, ";\n");
}

static void emitReport(struct AstNode* rstmt){
	struct ReportStatement* rStat = (struct ReportStatement*)rstmt;	

	fprintf(vhdlFile, "%creport ", emitIndent());

	if(rStat->stringExpr){
		eStat.ignore = true;

		struct StringExpr* stexp = (struct StringExpr*)rStat->stringExpr;
		fprintf(vhdlFile, "%s", stexp->literal);
	}

	int severity = rStat->severity.level;
	if(severity != SEVERITY_NULL) {
		fprintf(vhdlFile, " severity ");
		
		switch(severity) {
			case SEVERITY_NOTE:
				fprintf(vhdlFile, "note");
				break;
			case SEVERITY_WARNING:
				fprintf(vhdlFile, "warning");
				break;
			case SEVERITY_ERROR:
				fprintf(vhdlFile, "error");
				break;
			case SEVERITY_FAILURE:
				fprintf(vhdlFile, "failure");
				break;
			default:
				break;
		}
	}

	fprintf(vhdlFile, ";\n");
}

static void emitNull(struct AstNode* nstmt){
	fprintf(vhdlFile, "%cnull;\n", emitIndent());
}

static void emitTypeDeclaration(struct AstNode* tDecl){
	struct TypeDecl* typeDecl = (struct TypeDecl*) tDecl;
	
	char* typeName = typeDecl->typeName->value;
	fprintf(vhdlFile, "%ctype %s is (", emitIndent(), typeName);		
	
	eStat.list = true;
}

static void emitTypeDeclarationClose(struct AstNode* tDecl){
	//overwrite that last comma
	fseek(vhdlFile, -1, SEEK_CUR);

	fprintf(vhdlFile, ");\n");		
	eStat.list = false;
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

	if(varAssign->op){
		if(strcmp(varAssign->op, "+=") == 0) {
			fprintf(vhdlFile, ":= %s +", target);
			memcpy(eStat.assignmentOp, "\0", 1);
		} else if(strcmp(varAssign->op, "++") == 0) {
			fprintf(vhdlFile, ":= %s + 1;\n", target);
			memcpy(eStat.assignmentOp, "\0", 1);
		} else if(strcmp(varAssign->op, "-=") == 0) {
			fprintf(vhdlFile, ":= %s -", target);
			memcpy(eStat.assignmentOp, "\0", 1);
		} else if(strcmp(varAssign->op, "--") == 0) {
			fprintf(vhdlFile, ":= %s + 1;\n", target);
			memcpy(eStat.assignmentOp, "\0", 1);
		} else if(strcmp(varAssign->op, "*=") == 0) {
			fprintf(vhdlFile, ":= %s *", target);
			memcpy(eStat.assignmentOp, "\0", 1);
		} else if(strcmp(varAssign->op, "/=") == 0) {
			fprintf(vhdlFile, ":= %s /", target);
			memcpy(eStat.assignmentOp, "\0", 1);
		}
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

static void emitBinaryOp(char* bop){
	
	if(strcmp(bop, "!=") == 0){
		fprintf(vhdlFile, " /= ");
	} else if (strcmp(bop, "==") == 0) {
		fprintf(vhdlFile, " = ");
	} else {
		fprintf(vhdlFile, " %s ", bop);
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

		case STRING_EXPR: {
			struct StringExpr* stexp = (struct StringExpr*)expr;
			fprintf(vhdlFile, "%s", stexp->literal);
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
			 emitBinaryOp(bexp->op);
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

static void emitRange(struct AstNode* rstmt){
	struct Range* range = (struct Range*)rstmt;

	if(range->left) {
		fprintf(vhdlFile, " ");
		emitSubExpression(range->left);
	}

	if(range->right) {
		if(range->descending) fprintf(vhdlFile, " downto ");
		else fprintf(vhdlFile, " to ");

		emitSubExpression(range->right);
	}
}

static void emitExpression(struct Expression* expr){

	if(eStat.ignore != true) {	
		fprintf(vhdlFile, "%s ", eStat.assignmentOp);	
		emitSubExpression(expr);
		if(eStat.close){
			fprintf(vhdlFile, ";\n");
		}
		if(eStat.list){
			fprintf(vhdlFile, ",");
		}
	}

	eStat.incoming = 0;
	eStat.ignore = 0;
	eStat.close = 0;
	eStat.assignmentOp[0] = 0;
}

static void emitSpecial(struct AstNode* node){
	
	switch(node->type){
	
		case AST_GENERIC:
			emitGenericDeclarationSpecial(node);
			break;
		
		case AST_IF:
			emitElse(node);
			break;

		case AST_CASE:
			emitWhenSpecial(node);
			break;
	
		default:
			break;
	}
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
		
		case AST_IF:
			emitIfClose(node);
         break;

		case AST_SWITCH:
			emitCaseClose(node);
			break;

		case AST_CASE:
			emitWhenClose(node);
			break;
	
		case AST_WHILE:
		case AST_FOR:
		case AST_LOOP:
			emitLoopClose(node);
			break;
		
		case AST_TDECL:
			emitTypeDeclarationClose(node);

		default:
			break;
	}
}

static void emitOpen(struct AstNode* node){

	switch(node->type){
		
		case AST_GENERIC:
			emitGenericDeclarationOpen(node);
			break;
		
		case AST_PORT:
			emitPortDeclarationOpen(node);
			break;
		
		case AST_ARCHITECTURE:
			emitArchitectureDeclarationOpen(node);
			break;
		
		case AST_PROCESS:
			emitProcessOpen(node);
			break;
		
		case AST_IF:
      case AST_ELSIF:
			emitIfOpen(node);
         break;

		case AST_SWITCH:
			emitCaseOpen(node);
			break;

		case AST_CASE:
			emitWhenOpen(node);
			break;
	
		case AST_WHILE:
		case AST_FOR:
			emitLoopOpen(node);
			break;
		
		default:
			break;
	}
}

static void emitDefault(struct AstNode* node){

	switch(node->type){
		
      case AST_PROGRAM:
         break;

      case AST_USE:
         emitUseStatement(node);
         break;

      case AST_ENTITY:
         emitEntityDeclaration(node);
         break;

      case AST_ARCHITECTURE:
         emitArchitectureDeclaration(node);
         break;

      case AST_GENERIC:
         emitGenericDeclaration(node);
         break;

      case AST_PORT:
         emitPortDeclaration(node);
         break;

      case AST_PROCESS:
			emitProcess(node);
         break;

      case AST_FOR:
			emitForLoop(node);
         break;

		case AST_IF:
      case AST_ELSIF:
			emitIfStatement(node);
         break;

		case AST_SWITCH:
			emitCaseStatement(node);
			break;

		case AST_CASE:
			emitWhenStatement(node);
			break;
	
      case AST_LOOP:
			emitLoop(node);
         break;

      case AST_WAIT:
			emitWait(node);
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

      case AST_TDECL:
         emitTypeDeclaration(node);
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
			emitRange(node);
         break;

      case AST_ASSERT:
			emitAssert(node);
         break;

      case AST_REPORT:
			emitReport(node);
         break;

      case AST_NULL:
			emitNull(node);
         break;

      default:
			printf("Emitter: Unhandled AST node: %d\r\n", node->type); 
         break;
	}
}

void TranspileProgram(struct Program* prog, char* fileName){

	//setup block
	struct OperationBlock opBlk = {
		.doDefaultOp		= emitDefault,
		.doOpenOp			= emitOpen,
		.doCloseOp			= emitClose,
		.doSpecialOp		= emitSpecial,
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
