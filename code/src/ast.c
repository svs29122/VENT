#include <stdlib.h>
#include <stdio.h>

#include <ast.h>

static void noOp(void* p){return;}
static void noDefaultOp(struct AstNode* p){return;}

struct OperationBlock* InitOperationBlock(void){
	struct OperationBlock* op = malloc(sizeof(struct OperationBlock));	

	op->doDefaultOp					= noDefaultOp;
	op->doCloseOp						= noDefaultOp;
	op->doSpecialOp					= noDefaultOp;
	op->doExpressionOp				= noOp;
	op->doProgOp 						= noOp;
	op->doBlockArrayOp 				= noOp;
	op->doUseStatementOp				= noOp;
	op->doEntityDeclOp				= noOp;
	op->doArchDeclOp					= noOp;
	op->doArchDeclOpenOp				= noOp;
	op->doPortDeclOp					= noOp;
	op->doPortDeclOpenOp				= noOp;
	op->doSignalDeclOp				= noOp;
	op->doVariableDeclOp				= noOp;
	op->doSignalAssignOp				= noOp;
	op->doVariableAssignOp			= noOp;
	op->doAssignmentOp	 			= noOp;
	op->doForStatementOp				= noOp;
	op->doForOpenOp 					= noOp;
	op->doIfStatementOp 				= noOp;
	op->doIfStatementElseOp			= noOp;
	op->doIfStatementElsifOp		= noOp;
	op->doLoopStatementOp			= noOp;
	op->doWaitStatementOp			= noOp;
	op->doWhileStatementOp			= noOp;
	op->doWhileOpenOp 				= noOp;
	op->doProcessOp					= noOp;
	op->doProcessOpenOp				= noOp;
	op->doIdentifierOp 				= noOp;
	op->doPortModeOp 					= noOp;
	op->doDataTypeOp 					= noOp;
	op->doRangeOp	 					= noOp;

	return op;
}

//forward declarations
static void walkSequentialStatements(Dba* stmts, struct OperationBlock* op);

static void walkVariableDeclaration(struct VariableDecl* varDecl, struct OperationBlock* op){
	op->doDefaultOp(&(varDecl->self));
	if(varDecl->name){
		op->doDefaultOp(&(varDecl->name->self.root));
	}
	if(varDecl->dtype){
		op->doDefaultOp(&(varDecl->dtype->self));
	}	
	if(varDecl->expression){
		op->doExpressionOp((void*)varDecl->expression);
	}
	op->doCloseOp(&(varDecl->self));
}

static void walkSignalDeclaration(struct SignalDecl* sigDecl, struct OperationBlock* op){
	op->doDefaultOp(&(sigDecl->self));
	if(sigDecl->name){
		op->doDefaultOp(&(sigDecl->name->self.root));
	}
	if(sigDecl->dtype){
		op->doDefaultOp(&(sigDecl->dtype->self));
	}	
	if(sigDecl->expression){
		op->doExpressionOp((void*)sigDecl->expression);
	}
	op->doCloseOp(&(sigDecl->self));
}

static void walkForStatement(struct ForStatement* fStmt, struct OperationBlock* op){
	op->doDefaultOp(&(fStmt->self));
	if(fStmt->parameter){
		op->doDefaultOp(&(fStmt->parameter->self.root));
	}
	if(fStmt->range){
		op->doDefaultOp(&(fStmt->range->self));
	}
	op->doForOpenOp((void*)fStmt);
	if(fStmt->statements){
		walkSequentialStatements(fStmt->statements, op);
	}
	op->doCloseOp(&(fStmt->self));
}

static void walkIfStatement(struct IfStatement* ifStmt, struct OperationBlock* op){
	op->doDefaultOp(&(ifStmt->self));
	if(ifStmt->antecedent){
		op->doExpressionOp(ifStmt->antecedent);
	}
	if(ifStmt->consequentStatements){
		walkSequentialStatements(ifStmt->consequentStatements, op);
	}
	op->doCloseOp(&(ifStmt->self));
	if(ifStmt->elsif){
		walkIfStatement(ifStmt->elsif, op);
		op->doIfStatementElsifOp((void*)ifStmt->elsif);
	}	
	if(ifStmt->alternativeStatements){
		op->doSpecialOp(&(ifStmt->self));
		walkSequentialStatements(ifStmt->alternativeStatements, op);
	}
}

static void walkLoopStatement(struct LoopStatement* lStmt, struct OperationBlock* op){
	op->doDefaultOp(&(lStmt->self));
	if(lStmt->statements){
		walkSequentialStatements(lStmt->statements, op);
	}
	op->doCloseOp((void*)lStmt);
}

static void walkWhileStatement(struct WhileStatement* wStmt, struct OperationBlock* op){
	op->doDefaultOp(&(wStmt->self));
	if(wStmt->condition){
		op->doExpressionOp(wStmt->condition);
	}
	op->doWhileOpenOp((void*)wStmt);
	if(wStmt->statements){
		walkSequentialStatements(wStmt->statements, op);
	}
	op->doCloseOp(&(wStmt->self));
}

static void walkWaitStatement(struct WaitStatement* wStmt, struct OperationBlock* op){
	op->doDefaultOp(&(wStmt->self));
	if(wStmt->sensitivityList){
		op->doDefaultOp(&(wStmt->sensitivityList->self.root));
	}
	if(wStmt->condition){
		op->doExpressionOp(wStmt->condition);
	}
	if(wStmt->time){
		op->doExpressionOp(wStmt->time);
	}
}

static void walkVariableAssignment(struct VariableAssign* varAssign, struct OperationBlock* op){
	op->doDefaultOp(&(varAssign->self));
	if(varAssign->target){
		op->doDefaultOp(&(varAssign->target->self.root));
	}
	if(varAssign->op){
		op->doSpecialOp(&(varAssign->self));
	}
	if(varAssign->expression){
		op->doExpressionOp((void*)varAssign->expression);
	}
	op->doCloseOp(&(varAssign->self));
}

static void walkSignalAssignment(struct SignalAssign* sigAssign, struct OperationBlock* op){
	op->doDefaultOp(&(sigAssign->self));
	if(sigAssign->target){
		op->doDefaultOp(&(sigAssign->target->self.root));
	}
	if(sigAssign->expression){
		op->doExpressionOp((void*)sigAssign->expression);
	}
	op->doCloseOp(&(sigAssign->self));
}

static void walkSequentialStatements(Dba* stmts, struct OperationBlock* op){
	for(int i=0; i < BlockCount(stmts); i++){
		struct SequentialStatement* qstmt = (struct SequentialStatement*) ReadBlockArray(stmts, i);
		switch(qstmt->type) {
			case FOR_STATEMENT: {
				walkForStatement(&(qstmt->as.forStatement), op);
				break;
			}
		
			case IF_STATEMENT: {
				walkIfStatement(&(qstmt->as.ifStatement), op);
				break;
			}
		
			case LOOP_STATEMENT: {
				walkLoopStatement(&(qstmt->as.loopStatement), op);
				break;
			}

			case QSIGNAL_ASSIGNMENT: {
				walkSignalAssignment(&(qstmt->as.signalAssignment), op);
				break;
			}

			case VARIABLE_ASSIGNMENT: {
				walkVariableAssignment(&(qstmt->as.variableAssignment), op);
				break;
			}

			case WAIT_STATEMENT: {
				walkWaitStatement(&(qstmt->as.waitStatement), op);
				break;
			}
		
			case WHILE_STATEMENT: {
				walkWhileStatement(&(qstmt->as.whileStatement), op);
				break;
			}
		
			default:
				break;
		}
	}
	op->doBlockArrayOp((void*)stmts);
}

static void walkDeclarations(Dba* decls, struct OperationBlock* op){
	for(int i=0; i < BlockCount(decls); i++){
		struct Declaration* decl = (struct Declaration*) ReadBlockArray(decls, i);
		switch (decl->type){
			case SIGNAL_DECLARATION: {
				walkSignalDeclaration(&(decl->as.signalDeclaration), op);
				break;
			}		

			case VARIABLE_DECLARATION: {
				walkVariableDeclaration(&(decl->as.variableDeclaration), op);
				break;
			}
	
			default:
				break;
		}
	}
	op->doBlockArrayOp((void*)decls);
}

static void walkProcessStatement(struct Process* proc, struct OperationBlock* op){
	op->doDefaultOp(&(proc->self));
	if(proc->sensitivityList){
		op->doDefaultOp(&(proc->sensitivityList->self.root));
	}
	if(proc->declarations){
		walkDeclarations(proc->declarations, op);
	}
	op->doProcessOpenOp((void*)proc);
	if(proc->statements){
		walkSequentialStatements(proc->statements, op);
	}
	op->doCloseOp(&(proc->self));
}

static void walkConcurrentStatements(Dba* stmts, struct OperationBlock* op){
	for(int i=0; i < BlockCount(stmts); i++){
		struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*) ReadBlockArray(stmts, i) ;
		switch(cstmt->type) {
			case SIGNAL_ASSIGNMENT: {
				walkSignalAssignment(&(cstmt->as.signalAssignment), op);
				break;
			}
		
			case PROCESS: {
				walkProcessStatement(&(cstmt->as.process), op);
				break;
			}

			default:
				break;
		}
	}
	op->doBlockArrayOp((void*)stmts);
}

static void walkArchitecture(struct ArchitectureDecl* archDecl, struct OperationBlock* op){
	op->doDefaultOp(&(archDecl->self));
	if(archDecl->archName){
		op->doDefaultOp(&(archDecl->archName->self.root));
	}
	if(archDecl->entName){
		op->doDefaultOp(&(archDecl->entName->self.root));
	}
	if(archDecl->declarations){
		walkDeclarations(archDecl->declarations, op);
	}
	op->doArchDeclOpenOp((void*)archDecl);
	if(archDecl->statements){
		walkConcurrentStatements(archDecl->statements, op);
	}
	op->doCloseOp(&(archDecl->self));
}

static void walkEntity(struct EntityDecl* entDecl, struct OperationBlock* op){
	op->doDefaultOp(&(entDecl->self));
	if(entDecl->name){
		op->doDefaultOp(&(entDecl->name->self.root));
	}
	if(entDecl->ports){
		Dba* ports = entDecl->ports;
		op->doPortDeclOpenOp((void*)ports);
		for(int i=0; i < BlockCount(ports); i++){
			struct PortDecl* portDecl = (struct PortDecl*) ReadBlockArray(ports, i);
			op->doDefaultOp(&(portDecl->self));
			if(portDecl->name){
				op->doDefaultOp(&(portDecl->name->self.root));
			}
			if(portDecl->pmode){
				op->doDefaultOp(&(portDecl->pmode->self));
			}
			if(portDecl->dtype){
				op->doDefaultOp(&(portDecl->dtype->self));
			}	
			op->doCloseOp(&(portDecl->self));
		}
		op->doBlockArrayOp((void*)ports);	
	}
	op->doCloseOp((void*)entDecl);
}

static void walkDesignUnits(Dba* arr, struct OperationBlock* op){
	for(int i=0; i < BlockCount(arr); i++){
		struct DesignUnit* unit = (struct DesignUnit*) ReadBlockArray(arr, i);
		switch(unit->type){
			case ENTITY: {
				walkEntity(&(unit->as.entity), op);
				break;
			}
			case ARCHITECTURE: {					
				walkArchitecture(&(unit->as.architecture), op);
				break;
			}
			default:
				break;
		}		
	}
	op->doBlockArrayOp((void*)arr);	
}

static void walkUseStatements(Dba* arr, struct OperationBlock* op){
	for(int i=0; i < BlockCount(arr); i++){
		struct UseStatement* stmt = (struct UseStatement*) ReadBlockArray(arr, i);
		if(stmt){
			op->doDefaultOp(&(stmt->self));
		}
	}
	op->doBlockArrayOp((void*)arr);
}

void WalkTree(struct Program *prog, struct OperationBlock* op){
	if(prog){
		op->doDefaultOp(&(prog->self));
		if(prog->useStatements){
			walkUseStatements(prog->useStatements, op);
		}
		if(prog->units){
			walkDesignUnits(prog->units, op);
		}
		// TODO: used in FreeProgram
		op->doProgOp((void*)prog);
	}
}
