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
	op->doSignalAssignOp				= noOp;
	op->doProcessOp					= noOp;
	op->doProcessCloseOp				= noOp;
	op->doIdentifierOp 				= noOp;
	op->doPortModeOp 					= noOp;
	op->doDataTypeOp 					= noOp;
	op->doExpressionOp				= noOp;

	return op;
}

static void walkSequentialStatements(Dba* stmts, struct OperationBlock* op){
	for(int j=0; j < stmts->count; j++){
		struct SequentialStatement* qstmt = (struct SequentialStatement*)(stmts->block + (j * stmts->blockSize));
		switch(qstmt->type) {
			case SEQ_SIGNAL_ASSIGNMENT: {
				struct SignalAssign* sigAssign = &(qstmt->as.signalAssignment); 
				op->doSignalAssignOp((void*)sigAssign);
				if(sigAssign->target){
					op->doIdentifierOp((void*)sigAssign->target);
				}
				if(sigAssign->expression){
					op->doExpressionOp((void*)sigAssign->expression);
				}
				break;
			}
		
			default:
				break;
		}
	}
	op->doBlockArrayOp((void*)stmts);
}

static void walkConcurrentStatements(Dba* stmts, struct OperationBlock* op){
	for(int j=0; j < stmts->count; j++){
		struct ConcurrentStatement* cstmt = (struct ConcurrentStatement*)(stmts->block + (j * stmts->blockSize));
		switch(cstmt->type) {
			case SIGNAL_ASSIGNMENT: {
				struct SignalAssign* sigAssign = &(cstmt->as.signalAssignment); 
				op->doSignalAssignOp((void*)sigAssign);
				if(sigAssign->target){
					op->doIdentifierOp((void*)sigAssign->target);
				}
				if(sigAssign->expression){
					op->doExpressionOp((void*)sigAssign->expression);
				}
				break;
			}
		
			case PROCESS: {
				struct Process* proc = &(cstmt->as.process);
				op->doProcessOp((void*)proc);
				if(proc->sensitivityList){
					op->doIdentifierOp((void*)proc->sensitivityList);
				}
				if(proc->statements){
					walkSequentialStatements(proc->statements, op);
				}
				op->doProcessCloseOp((void*)proc);
				break;
			}

			default:
				break;
		}
	}
	op->doBlockArrayOp((void*)stmts);
}

static void walkDeclarations(Dba* decls, struct OperationBlock* op){
	for(int j=0; j < decls->count; j++){
		struct SignalDecl* sigDecl = (struct SignalDecl*)(decls->block + (j * decls->blockSize));
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
	op->doBlockArrayOp((void*)decls);
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
		for(int j=0; j < ports->count; j++){
			struct PortDecl* portDecl = (struct PortDecl*)(ports->block + (j * ports->blockSize));
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
	for(int i=0; i < arr->count; i++){
		struct DesignUnit* unit = (struct DesignUnit*)(arr->block + (i * arr->blockSize));
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
	for(int i=0; i < arr->count; i++){
		struct UseStatement* stmt = (struct UseStatement*)(arr->block + (i * arr->blockSize));
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
