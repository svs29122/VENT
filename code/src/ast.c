#include <stdlib.h>
#include <stdio.h>

#include <ast.h>

void noOp(struct AstNode* p){return;}
void noExpOp(struct Expression* p){return;}
void noBlkOp(struct DynamicBlockArray* p){return;}

static void getOperationBlockReady(struct OperationBlock* op){

	// if NULL set to noOp
	if(!(op->doDefaultOp)) op->doDefaultOp = noOp;
	if(!(op->doOpenOp)) op->doOpenOp = noOp;
	if(!(op->doCloseOp)) op->doCloseOp = noOp;
	if(!(op->doSpecialOp)) op->doSpecialOp = noOp;
	if(!(op->doExpressionOp)) op->doExpressionOp = noExpOp;
	if(!(op->doBlockArrayOp)) op->doBlockArrayOp = noBlkOp;;
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
		op->doExpressionOp(varDecl->expression);
	}
	op->doCloseOp(&(varDecl->self));
}

static void walkExpressionList(struct ExpressionList* eList, struct OperationBlock* op){
	struct ExpressionList* currList = eList;
	do {
		if(currList->expression){
			op->doExpressionOp(currList->expression);
		}	
		currList = currList->next;
	} while(currList);
}

static void walkTypeDeclaration(struct TypeDecl* typeDecl, struct OperationBlock* op){
	op->doDefaultOp(&(typeDecl->self));
	if(typeDecl->typeName){
		op->doDefaultOp(&(typeDecl->typeName->self.root));
	}
	if(typeDecl->enumList){
		walkExpressionList(typeDecl->enumList, op);

		op->doSpecialOp(&(typeDecl->self));
	}
	op->doCloseOp(&(typeDecl->self));
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
		op->doExpressionOp(sigDecl->expression);
	}
	op->doCloseOp(&(sigDecl->self));
}

static void walkReportStatement(struct ReportStatement* rStmt, struct OperationBlock* op){
	op->doDefaultOp(&(rStmt->self));
	if(rStmt->stringExpr){
		op->doExpressionOp(rStmt->stringExpr);
	}
	if(rStmt->severity.level != SEVERITY_NULL){
		op->doSpecialOp(&(rStmt->self));
	}
	op->doCloseOp(&(rStmt->self));
}

static void walkAssertStatement(struct AssertStatement* aStmt, struct OperationBlock* op){
	op->doDefaultOp(&(aStmt->self));
	if(aStmt->condition){
		op->doExpressionOp(aStmt->condition);
	}
	walkReportStatement(&(aStmt->report), op);
	op->doCloseOp(&(aStmt->self));
}

static void walkNullStatement(struct NullStatement* nullStmt, struct OperationBlock* op){
	op->doDefaultOp(&(nullStmt->self));
}

static void walkCaseStatements(Dba* cases, struct OperationBlock* op){
	for(int i=0; i<BlockCount(cases); i++){
		struct CaseStatement* aCase = (struct CaseStatement*) ReadBlockArray(cases, i);
		op->doDefaultOp(&(aCase->self));

		struct Choice* choice = aCase->choices;		
		while(choice != NULL){
			switch(choice->type) {

				case CHOICE_NUMEXPR: {
					op->doExpressionOp(choice->as.numExpr);
					break;
				}
	
				case CHOICE_RANGE: {
					op->doDefaultOp(&(choice->as.range->self));
					break;
				}	

				default:
					break;
			}
			choice = choice->nextChoice;
		}
		op->doSpecialOp(&(aCase->self));

		if(aCase->statements){
			walkSequentialStatements(aCase->statements, op);
		}
		op->doCloseOp(&(aCase->self));
	}
	op->doBlockArrayOp(cases);
}

static void walkSwitchStatement(struct SwitchStatement* switchStmt, struct OperationBlock* op){
	op->doDefaultOp(&(switchStmt->self));
	if(switchStmt->expression){
		op->doExpressionOp(switchStmt->expression);
	}
	if(switchStmt->cases){
		walkCaseStatements(switchStmt->cases, op);
	}
	op->doCloseOp(&(switchStmt->self));
}

static void walkForStatement(struct ForStatement* forStmt, struct OperationBlock* op){
	op->doDefaultOp(&(forStmt->self));
	if(forStmt->parameter){
		op->doDefaultOp(&(forStmt->parameter->self.root));
	}
	if(forStmt->range){
		op->doDefaultOp(&(forStmt->range->self));
	}
	op->doOpenOp(&(forStmt->self));
	if(forStmt->statements){
		walkSequentialStatements(forStmt->statements, op);
	}
	op->doCloseOp(&(forStmt->self));
}

static void walkIfStatement(struct IfStatement* ifStmt, struct OperationBlock* op){
	op->doDefaultOp(&(ifStmt->self));
	if(ifStmt->antecedent){
		op->doExpressionOp(ifStmt->antecedent);
	}
	op->doOpenOp(&(ifStmt->self));
	if(ifStmt->consequentStatements){
		walkSequentialStatements(ifStmt->consequentStatements, op);
	}
	if(ifStmt->elsif){
		walkIfStatement(ifStmt->elsif, op);
		op->doSpecialOp(&(ifStmt->elsif->self));
	}	
	if(ifStmt->alternativeStatements){
		op->doSpecialOp(&(ifStmt->self));
		walkSequentialStatements(ifStmt->alternativeStatements, op);
	}
	op->doCloseOp(&(ifStmt->self));
}

static void walkLoopStatement(struct LoopStatement* lStmt, struct OperationBlock* op){
	op->doDefaultOp(&(lStmt->self));
	if(lStmt->statements){
		walkSequentialStatements(lStmt->statements, op);
	}
	op->doCloseOp(&(lStmt->self));
}

static void walkWhileStatement(struct WhileStatement* wStmt, struct OperationBlock* op){
	op->doDefaultOp(&(wStmt->self));
	if(wStmt->condition){
		op->doExpressionOp(wStmt->condition);
	}
	op->doOpenOp(&(wStmt->self));
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
		op->doExpressionOp(varAssign->expression);
	}
	op->doCloseOp(&(varAssign->self));
}

static void walkSignalAssignment(struct SignalAssign* sigAssign, struct OperationBlock* op){
	op->doDefaultOp(&(sigAssign->self));
	if(sigAssign->target){
		op->doDefaultOp(&(sigAssign->target->self.root));
	}
	if(sigAssign->expression){
		op->doExpressionOp(sigAssign->expression);
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

			case NULL_STATEMENT: {
				walkNullStatement(&(qstmt->as.nullStatement), op);
				break;
			}

			case ASSERT_STATEMENT: {
				walkAssertStatement(&(qstmt->as.assertStatement), op);
				break;
			}

			case REPORT_STATEMENT: {
				walkReportStatement(&(qstmt->as.reportStatement), op);
				break;
			}

			case SWITCH_STATEMENT: {
				walkSwitchStatement(&(qstmt->as.switchStatement), op);
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
	op->doBlockArrayOp(stmts);
}

static void walkDeclarations(Dba* decls, struct OperationBlock* op){
	for(int i=0; i < BlockCount(decls); i++){
		struct Declaration* decl = (struct Declaration*) ReadBlockArray(decls, i);
		switch (decl->type){
			
			case TYPE_DECLARATION: {
				walkTypeDeclaration(&(decl->as.typeDeclaration), op);
				break;
			}
		
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
	op->doBlockArrayOp(decls);
}

static void walkProcessStatement(struct Process* proc, struct OperationBlock* op){
	op->doDefaultOp(&(proc->self));
	if(proc->sensitivityList){
		op->doDefaultOp(&(proc->sensitivityList->self.root));
	}
	if(proc->declarations){
		walkDeclarations(proc->declarations, op);
	}
	op->doOpenOp(&(proc->self));
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
	op->doBlockArrayOp(stmts);
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
	op->doOpenOp(&(archDecl->self));
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
		op->doOpenOp(&(entDecl->self));
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
		op->doBlockArrayOp(ports);	
	}
	op->doCloseOp(&(entDecl->self));
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
	op->doBlockArrayOp(arr);	
}

static void walkUseStatements(Dba* arr, struct OperationBlock* op){
	for(int i=0; i < BlockCount(arr); i++){
		struct UseStatement* stmt = (struct UseStatement*) ReadBlockArray(arr, i);
		if(stmt){
			op->doDefaultOp(&(stmt->self));
		}
	}
	op->doBlockArrayOp(arr);
}

void WalkTree(struct Program *prog, struct OperationBlock* op){
	getOperationBlockReady(op);
	
	if(prog){
		op->doDefaultOp(&(prog->self));
		if(prog->useStatements){
			walkUseStatements(prog->useStatements, op);
		}
		if(prog->units){
			walkDesignUnits(prog->units, op);
		}
		op->doSpecialOp(&(prog->self));
	}
}
