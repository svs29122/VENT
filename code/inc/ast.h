#ifndef INC_AST_H
#define INC_AST_H

#include "token.h"
#include "dba.h"

struct Program;

typedef void (*astNodeOpPtr) (void*);

struct OperationBlock* InitOperationBlock(void);
void WalkTree(struct Program* prog, struct OperationBlock* op);

struct OperationBlock {
	astNodeOpPtr doProgOp;
	astNodeOpPtr doBlockArrayOp;
	astNodeOpPtr doUseStatementOp;
	astNodeOpPtr doDesignUnitOp;
	astNodeOpPtr doEntityDeclOp;
	astNodeOpPtr doEntityDeclCloseOp;
	astNodeOpPtr doArchDeclOp;
	astNodeOpPtr doArchDeclOpenOp;
	astNodeOpPtr doArchDeclCloseOp;
	astNodeOpPtr doPortDeclOp;
	astNodeOpPtr doPortDeclOpenOp;
	astNodeOpPtr doPortDeclCloseOp;
	astNodeOpPtr doSignalDeclOp;
	astNodeOpPtr doVariableDeclOp;
	astNodeOpPtr doSignalAssignOp;
	astNodeOpPtr doVariableAssignOp;
	astNodeOpPtr doWaitStatementOp;
	astNodeOpPtr doWhileStatementOp;
	astNodeOpPtr doWhileOpenOp;
	astNodeOpPtr doWhileCloseOp;
	astNodeOpPtr doProcessOp;
	astNodeOpPtr doProcessOpenOp;
	astNodeOpPtr doProcessCloseOp;
	astNodeOpPtr doIdentifierOp;
	astNodeOpPtr doPortModeOp;
	astNodeOpPtr doDataTypeOp;
	astNodeOpPtr doExpressionOp;
};

enum ExpressionType{
		BINARY_EXPR,
		UNARY_EXPR,
		GROUPED_EXPR,
		NAME_EXPR,
		NUM_EXPR,
		PHYLIT_EXPR,
		CHAR_EXPR,
		STRING_EXPR,
		AGGREGATE_EXPR,
		Q_EXPR,
		NEW_EXPR,
		CALL_EXPR,
};

struct Expression {
#ifdef DEBUG
	struct Token token;
#endif
	enum ExpressionType type;
};

struct BinaryExpr {
	struct Expression self;
	struct Expression* left;
	struct Expression* right;
	char* op;
};

struct UnaryExpr {
	struct Expression self;
	struct Expression* right;
	char* op;
};

struct NameExpr {
	struct Expression self;
	struct Identifier* name;
};

struct CharExpr {
	struct Expression self;
	char* literal;
};

struct NumExpr {
	struct Expression self;
	char* literal;
};

struct Identifier {
	struct Expression self;
	char* value;
	//TODO: perhaps identifiers can have pointers to other identifiers?
	//this would be an easy way to implement comma separated lists of 
	//identifiers e.g. in portDecls and proc sensitivity lists
};

struct DataType {
#ifdef DEBUG
	struct Token token;
#endif
	char* value;
};

struct Label {
#ifdef DEBUG
	struct Token token;
#endif
	char* value;
};

struct PortMode {
#ifdef DEBUG
	struct Token token;
#endif
	char* value;
};

struct VariableDecl {
#ifdef DEBUG
	struct Token token; // the sig keyword
#endif
	//TODO: need to add support for , separated identifier list
	struct Identifier *name;
	struct DataType* dtype;
	struct Expression* expression;
};

struct SignalDecl {
#ifdef DEBUG
	struct Token token; // the sig keyword
#endif
	//need to add support for , separated identifier list
	struct Identifier *name;
	struct DataType* dtype;
	struct Expression* expression;
};

struct Declaration {
	enum {
		//PROCEDURE_DECLARATION,
		//PROCEDURE_BODY,
		//FUNCTION_DECLARATION,
		//FUNCTION_BODY,
		//TYPE_DECLARATION,
		//SUBTYPE_DECLARATION,
		//CONSTANT_DECLARATION,
		SIGNAL_DECLARATION,
		VARIABLE_DECLARATION,
		//FILE_DECLARATION,
		//COMPONENT_DECLARATION,
	} type;
	union {
		struct SignalDecl signalDeclaration;
		struct VariableDecl variableDeclaration;
	} as;
};

struct WhileStatement {
#ifdef DEBUG
	struct Token token; // the "while" token
#endif
	struct Label* label;
	struct Expression* condition;
	struct DynamicBlockArray* statements;
};

struct WaitStatement {
#ifdef DEBUG
	struct Token token; // the "wait" token
#endif
	struct Label* label;
	struct Identifier* sensitivityList;
	struct Expression* condition;
	struct Expression* time;
};

struct VariableAssign {
#ifdef DEBUG
	struct Token token; // the ":=" operator
#endif
	struct Label* label;
	struct Identifier* target;
	struct Expression* expression;
};

struct SignalAssign {
#ifdef DEBUG
	struct Token token; // the "<=" operator
#endif
	struct Label* label;
	struct Identifier* target;
	struct Expression* expression;
};

struct SequentialStatement {
	enum {
		QSIGNAL_ASSIGNMENT,
		VARIABLE_ASSIGNMENT,
		IF_STATEMENT,
		WAIT_STATEMENT,
		WHILE_STATEMENT,
		//CASE,
		//LOOP,
	} type;
	union {
		struct SignalAssign signalAssignment;
		struct VariableAssign variableAssignment;
		struct WaitStatement waitStatement;
		struct WhileStatement whileStatement;
	} as;
};

struct Process {
#ifdef DEBUG
	struct Token token; // the proc keyword
#endif
	struct Label label;
	struct Identifier* sensitivityList;
	struct DynamicBlockArray* declarations;
	struct DynamicBlockArray* statements;
};

struct ConcurrentStatement {
	enum {
		PROCESS,
		//INSTANTIATION,
		SIGNAL_ASSIGNMENT,
		//GENERATE,
		//ASSERT,
		//PROCEDURE_CALL,
		//BLOCK,
	} type;
	union {
		struct Process process;
		struct SignalAssign signalAssignment;
	} as;
};

struct ArchitectureDecl {
#ifdef DEBUG
	struct Token token; //the arch keyword
#endif
	struct Identifier* archName;
	struct Identifier* entName;
	struct DynamicBlockArray* declarations;
	struct DynamicBlockArray* statements;
};

struct PortDecl {
	//need to add support for , separated identifier list
	struct Identifier *name;
	struct PortMode* pmode;
	struct DataType* dtype; 
};

struct EntityDecl {
#ifdef DEBUG
	struct Token token; // the ent keyword
#endif
	struct Identifier* name;
	struct DynamicBlockArray* ports;
};

struct DesignUnit{
	enum {
		ENTITY,
		ARCHITECTURE,
		//PACKAGE,
		//PACKAGE_BODY,
		//CONFIGURATION
	} type;
	union {
		struct EntityDecl entity;
		struct ArchitectureDecl architecture;
		//PackageDecl;
		//PackageBodyDecl;
		//ConfigurationDecl; 
	} as;
};

struct UseStatement {
#ifdef DEBUG
	struct Token token;
#endif
	char* value;
};

struct Program {
	struct DynamicBlockArray* useStatements;
	struct DynamicBlockArray* units;
};

#endif //INC_AST_H
