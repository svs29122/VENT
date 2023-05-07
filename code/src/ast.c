#include <stdlib.h>
#include <stdio.h>

#include "ast.h"

static void noOp(void* p){return;}

struct OperationBlock* InitOperationBlock(void){
	struct OperationBlock* op = malloc(sizeof(struct OperationBlock));	

	op->doProgOp 						= noOp;
	op->doBlockArrayOp 				= noOp;
	op->doUseStatementOp				= noOp;
	op->doDesignUnitOp				= noOp;
	op->doEntityDeclOp				= noOp;
	op->doEntityDeclCloseOp			= noOp;
	op->doArchDeclOp					= noOp;
	op->doArchDeclOpenOp				= noOp;
	op->doArchDeclCloseOp			= noOp;
	op->doPortDeclOp					= noOp;
	op->doPortDeclOpenOp				= noOp;
	op->doPortDeclCloseOp			= noOp;
	op->doSignalDeclOp				= noOp;
	op->doVariableDeclOp				= noOp;
	op->doSignalAssignOp				= noOp;
	op->doVariableAssignOp			= noOp;
	op->doWaitStatementOp			= noOp;
	op->doWhileStatementOp			= noOp;
	op->doWhileOpenOp 				= noOp;
	op->doWhileCloseOp 				= noOp;
	op->doProcessOp					= noOp;
	op->doProcessOpenOp				= noOp;
	op->doProcessCloseOp				= noOp;
	op->doIdentifierOp 				= noOp;
	op->doPortModeOp 					= noOp;
	op->doDataTypeOp 					= noOp;
	op->doExpressionOp				= noOp;

	return op;
}

//forward declarations
static void walkSequentialStatements(Dba* stmts, struct OperationBlock* op);

static void walkVariableDeclaration(struct VariableDecl* varDecl, struct OperationBlock* op){
	op->doVariableDeclOp((void*)varDecl);
	if(varDecl->name){
		op->doIdentifierOp((void*)varDecl->name);
	}
	if(varDecl->dtype){
		op->doDataTypeOp((void*)varDecl->dtype);
	}	
	if(varDecl->expression){
		op->doExpressionOp((void*)varDecl->expression);
	}
}

static void walkSignalDeclaration(struct SignalDecl* sigDecl, struct OperationBlock* op){
	op->doSignalDeclOp((void*)sigDecl);
	if(sigDecl->name){
		op->doIdentifierOp((void*)sigDecl->name);
	}
	if(sigDecl->dtype){
		op->doDataTypeOp((void*)sigDecl->dtype);
	}	
	if(sigDecl->expression){
		op->doExpressionOp((void*)sigDecl->expression);
	}
}

static void walkWhileStatement(struct WhileStatement* wStmt, struct OperationBlock* op){
	op->doWhileStatementOp((void*)wStmt);
	if(wStmt->condition){
		op->doExpressionOp(wStmt->condition);
	}
	op->doWhileOpenOp((void*)wStmt);
	if(wStmt->statements){
		walkSequentialStatements(wStmt->statements, op);
	}
	op->doWhileCloseOp((void*)wStmt);
}

static void walkWaitStatement(struct WaitStatement* wStmt, struct OperationBlock* op){
	op->doWaitStatementOp((void*)wStmt);
	if(wStmt->sensitivityList){
		op->doIdentifierOp((void*)wStmt->sensitivityList);
	}
	if(wStmt->condition){
		op->doExpressionOp(wStmt->condition);
	}
	if(wStmt->time){
		op->doExpressionOp(wStmt->time);
	}
}

static void walkVariableAssignment(struct VariableAssign* varAssign, struct OperationBlock* op){
	op->doVariableAssignOp((void*)varAssign);
	if(varAssign->target){
		op->doIdentifierOp((void*)varAssign->target);
	}
	if(varAssign->expression){
		op->doExpressionOp((void*)varAssign->expression);
	}
}

static void walkSignalAssignment(struct SignalAssign* sigAssign, struct OperationBlock* op){
	op->doSignalAssignOp((void*)sigAssign);
	if(sigAssign->target){
		op->doIdentifierOp((void*)sigAssign->target);
	}
	if(sigAssign->expression){
		op->doExpressionOp((void*)sigAssign->expression);
	}
}

static void walkSequentialStatements(Dba* stmts, struct OperationBlock* op){
	for(int i=0; i < BlockCount(stmts); i++){
		struct SequentialStatement* qstmt = (struct SequentialStatement*) ReadBlockArray(stmts, i);
		switch(qstmt->type) {
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
	op->doProcessOp((void*)proc);
	if(proc->sensitivityList){
		op->doIdentifierOp((void*)proc->sensitivityList);
	}
	if(proc->declarations){
		walkDeclarations(proc->declarations, op);
	}
	op->doProcessOpenOp((void*)proc);
	if(proc->statements){
		walkSequentialStatements(proc->statements, op);
	}
	op->doProcessCloseOp((void*)proc);
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
	op->doArchDeclOp((void*)archDecl);
	if(archDecl->archName){
		op->doIdentifierOp((void*)archDecl->archName);
	}
	if(archDecl->entName){
		op->doIdentifierOp((void*)archDecl->entName);
	}
	if(archDecl->declarations){
		walkDeclarations(archDecl->declarations, op);
	}
	op->doArchDeclOpenOp((void*)archDecl);
	if(archDecl->statements){
		walkConcurrentStatements(archDecl->statements, op);
	}
	op->doArchDeclCloseOp((void*)archDecl);
}

static void walkEntity(struct EntityDecl* entDecl, struct OperationBlock* op){
	op->doEntityDeclOp((void*)entDecl);
	if(entDecl->name){
		op->doIdentifierOp((void*)entDecl->name);
	}
	if(entDecl->ports){
		Dba* ports = entDecl->ports;
		op->doPortDeclOpenOp((void*)ports);
		for(int i=0; i < BlockCount(ports); i++){
			struct PortDecl* portDecl = (struct PortDecl*) ReadBlockArray(ports, i);
			op->doPortDeclOp((void*)portDecl);
			if(portDecl->name){
				op->doIdentifierOp((void*)portDecl->name);
			}
			if(portDecl->pmode){
				op->doPortModeOp((void*)portDecl->pmode);
			}
			if(portDecl->dtype){
				op->doDataTypeOp((void*)portDecl->dtype);
			}	
		}
		op->doPortDeclCloseOp((void*)ports);
		op->doBlockArrayOp((void*)ports);	
	}
	op->doEntityDeclCloseOp((void*)entDecl);
}

static void walkDesignUnits(Dba* arr, struct OperationBlock* op){
	for(int i=0; i < BlockCount(arr); i++){
		struct DesignUnit* unit = (struct DesignUnit*) ReadBlockArray(arr, i);
		op->doDesignUnitOp((void*)unit);
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
			op->doUseStatementOp((void*)stmt);
		}
	}
	op->doBlockArrayOp((void*)arr);
}

void WalkTree(struct Program *prog, struct OperationBlock* op){
	if(prog){
		if(prog->useStatements){
			walkUseStatements(prog->useStatements, op);
		}
		if(prog->units){
			walkDesignUnits(prog->units, op);
		}
		op->doProgOp((void*)prog);
	}
}
