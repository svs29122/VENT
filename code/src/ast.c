#include <stdlib.h>
#include <stdio.h>

#include "ast.h"

void noOp(void* p){return;}

OperationBlock* initOperationBlock(void){
	OperationBlock* op = malloc(sizeof(OperationBlock));	

	op->doProgOp 						= noOp;
	op->doBlockArrayOp 				= noOp;
	op->doUseStatementOp				= noOp;
	op->doDesignUnitOp				= noOp;
	op->doEntityDeclOp				= noOp;
	op->doArchDeclOp					= noOp;
	op->doPortDeclOp					= noOp;
	op->doSignalDeclOp				= noOp;
	op->doSignalAssignOp				= noOp;
	op->doIdentifierOp 				= noOp;
	op->doPortModeOp 					= noOp;
	op->doDataTypeOp 					= noOp;
	op->doExpressionOp				= noOp;

	return op;
}

void WalkTree(Program *prog, OperationBlock* op){
	if(prog){
		if(prog->useStatements){
			Dba* arr = prog->useStatements;
			for(int i=0; i < arr->count; i++){
				UseStatement* stmt = (UseStatement*)(arr->block + (i * arr->blockSize));
				if(stmt){
					op->doUseStatementOp((void*)stmt);
				}
			}
			op->doBlockArrayOp((void*)arr);
		}
		if(prog->units){
			Dba* arr = prog->units;
			for(int i=0; i < arr->count; i++){
				DesignUnit* unit = (DesignUnit*)(arr->block + (i * arr->blockSize));
				op->doDesignUnitOp((void*)unit);
				switch(unit->type){
					case ENTITY: {
						EntityDecl* entDecl = &(unit->as.entity);
						op->doEntityDeclOp((void*)entDecl);
						if(entDecl->name){
							op->doIdentifierOp((void*)entDecl->name);
						}
						if(entDecl->ports){
							Dba* ports = entDecl->ports;
							for(int j=0; j < ports->count; j++){
								PortDecl* portDecl = (PortDecl*)(ports->block + (j * ports->blockSize));
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
							op->doBlockArrayOp((void*)ports);	
						}
						break;
					}
					case ARCHITECTURE: {					
						ArchitectureDecl* archDecl = &(unit->as.architecture);
						op->doArchDeclOp((void*)archDecl);
						if(archDecl->archName){
							op->doIdentifierOp((void*)archDecl->archName);
						}
						if(archDecl->entName){
							op->doIdentifierOp((void*)archDecl->entName);
						}
						if(archDecl->declarations){
							Dba* decls = archDecl->declarations;
							for(int j=0; j < decls->count; j++){
								SignalDecl* sigDecl = (SignalDecl*)(decls->block + (j * decls->blockSize));
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
						if(archDecl->statements){
							Dba* stmts = archDecl->statements;
							for(int j=0; j < stmts->count; j++){
								SignalAssign* sigAssign = (SignalAssign*)(stmts->block + (j * stmts->blockSize));
								op->doSignalAssignOp((void*)sigAssign);
								if(sigAssign->target){
									op->doIdentifierOp((void*)sigAssign->target);
								}
								if(sigAssign->expression){
									op->doExpressionOp((void*)sigAssign->expression);
								}
							}
							op->doBlockArrayOp((void*)stmts);
						}
						break;
					}
					default:
						break;
				}		
			}
			op->doBlockArrayOp((void*)arr);	
		}
		op->doProgOp((void*)prog);
	}
}
