#ifndef INC_AST_H
#define INC_AST_H

#include <stdbool.h>

#include "token.h"
#include "dba.h"

enum AstNodeType {
	AST_PROGRAM = 0,
	AST_USE,
	AST_ENTITY,
	AST_ARCHITECTURE,
	AST_PORT,
	AST_PROCESS,
	AST_FOR,
	AST_IF,
	AST_LOOP,
	AST_WAIT,
	AST_WHILE,
	AST_SASSIGN,
	AST_VASSIGN,
	AST_SDECL,
	AST_VDECL,
	AST_IDENTIFIER,
	AST_PMODE,
	AST_DTYPE,
	AST_RANGE,
	AST_EXPRESSION,
};

struct AstNode {
#ifdef DEBUG
	struct Token token;
#endif
	enum AstNodeType type;
};


struct Program;


struct OperationBlock* InitOperationBlock(void);
void WalkTree(struct Program* prog, struct OperationBlock* op);

//typedef void (*visitPtr) (void* node, enum AstNodeType type);
typedef void (*visitPtr) (struct AstNode*);

typedef void (*astNodeOpPtr) (void*);

struct OperationBlock {
	visitPtr doDefaultOp;
	visitPtr doOpenOp;
	visitPtr doCloseOp;
	visitPtr doSpecialOp;
	astNodeOpPtr doExpressionOp;
	astNodeOpPtr doProgOp;
	astNodeOpPtr doBlockArrayOp;
	astNodeOpPtr doUseStatementOp;
	astNodeOpPtr doEntityDeclOp;
	astNodeOpPtr doEntityDeclCloseOp;
	astNodeOpPtr doArchDeclOp;
	astNodeOpPtr doArchDeclOpenOp;
	astNodeOpPtr doArchDeclCloseOp;
	astNodeOpPtr doPortDeclOp;
	astNodeOpPtr doPortDeclOpenOp;
	astNodeOpPtr doPortDeclCloseOp;
	astNodeOpPtr doSignalDeclOp;
	astNodeOpPtr doSignalDeclCloseOp;
	astNodeOpPtr doVariableDeclOp;
	astNodeOpPtr doVariableDeclCloseOp;
	astNodeOpPtr doSignalAssignOp;
	astNodeOpPtr doSignalAssignCloseOp;
	astNodeOpPtr doVariableAssignOp;
	astNodeOpPtr doAssignmentOp;
	astNodeOpPtr doVariableAssignCloseOp;
	astNodeOpPtr doForStatementOp;
	astNodeOpPtr doForOpenOp;
	astNodeOpPtr doForCloseOp;
	astNodeOpPtr doIfStatementOp;
	astNodeOpPtr doIfStatementCloseOp;
	astNodeOpPtr doIfStatementElsifOp;
	astNodeOpPtr doIfStatementElseOp;
	astNodeOpPtr doLoopStatementOp;
	astNodeOpPtr doLoopCloseOp;
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
	astNodeOpPtr doRangeOp;
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
	struct AstNode root;
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

struct Range {
	struct AstNode self;

	struct Expression* left;
	bool descending;
	struct Expression* right;
};

struct DataType {
	struct AstNode self;

	char* value;
};

struct Label {
	struct AstNode self;

	char* value;
};

struct PortMode {
	struct AstNode self;

	char* value;
};

struct VariableDecl {
	struct AstNode self;

	//TODO: need to add support for , separated identifier list
	struct Identifier *name;
	struct DataType* dtype;
	struct Expression* expression;
};

struct SignalDecl {
	struct AstNode self;

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

struct ForStatement {
	struct AstNode self;

	struct Label* label;
	struct Identifier* parameter;
	struct Range* range;
	struct DynamicBlockArray* statements;
};

struct IfStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* antecedent;
	struct DynamicBlockArray* consequentStatements;
	struct DynamicBlockArray* alternativeStatements;
	struct IfStatement* elsif;
};

struct LoopStatement {
	struct AstNode self;

	struct Label* label;
	struct DynamicBlockArray* statements;
};

struct WhileStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* condition;
	struct DynamicBlockArray* statements;
};

struct WaitStatement {
	struct AstNode self;

	struct Label* label;
	struct Identifier* sensitivityList;
	struct Expression* condition;
	struct Expression* time;
};

struct VariableAssign {
	struct AstNode self;

	struct Label* label;
	struct Identifier* target;
	char* op;
	struct Expression* expression;
};

struct SignalAssign {
	struct AstNode self;

	struct Label* label;
	struct Identifier* target;
	struct Expression* expression;
};

struct SequentialStatement {
	enum {
		FOR_STATEMENT,
		IF_STATEMENT,
		LOOP_STATEMENT,
		QSIGNAL_ASSIGNMENT,
		VARIABLE_ASSIGNMENT,
		WAIT_STATEMENT,
		WHILE_STATEMENT,
		//CASE,
	} type;
	union {
		struct ForStatement forStatement;
		struct IfStatement ifStatement;
		struct LoopStatement loopStatement;
		struct SignalAssign signalAssignment;
		struct VariableAssign variableAssignment;
		struct WaitStatement waitStatement;
		struct WhileStatement whileStatement;
	} as;
};

struct Process {
	struct AstNode self;

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
	struct AstNode self;

	struct Identifier* archName;
	struct Identifier* entName;
	struct DynamicBlockArray* declarations;
	struct DynamicBlockArray* statements;
};

struct PortDecl {
	struct AstNode self;

	//need to add support for , separated identifier list
	struct Identifier *name;
	struct PortMode* pmode;
	struct DataType* dtype; 
};

struct EntityDecl {
	struct AstNode self;

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
	struct AstNode self;

	char* value;
};

struct Program {
	struct AstNode self;

	struct DynamicBlockArray* useStatements;
	struct DynamicBlockArray* units;
};

#endif //INC_AST_H
