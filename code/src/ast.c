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
	op->doPortDeclOp					= noOp;
	op->doIdentifierOp 				= noOp;
	op->doPortModeOp 					= noOp;
	op->doDataTypeOp 					= noOp;

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
						EntityDecl* entDecl = &(unit->decl.entity);
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
