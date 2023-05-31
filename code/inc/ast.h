#ifndef INC_AST_H
#define INC_AST_H

#include <stdbool.h>

#include "token.h"
#include "dba.h"

struct Program;
struct AstNode;
struct Expression;

typedef void (*astNodeOpPtr) (struct AstNode*);
typedef void (*expOpPtr) (struct Expression*);
typedef void (*blkOpPtr) (struct DynamicBlockArray*);

struct OperationBlock {
	astNodeOpPtr doDefaultOp;
	astNodeOpPtr doOpenOp;
	astNodeOpPtr doCloseOp;
	astNodeOpPtr doSpecialOp;
	expOpPtr doExpressionOp;
	blkOpPtr doBlockArrayOp;
};

void WalkTree(struct Program* prog, struct OperationBlock* op);

enum AstNodeType {
	AST_PROGRAM = 1,
	AST_USE,
	AST_ENTITY,
	AST_ARCHITECTURE,
	AST_PORT,
	AST_PROCESS,
	AST_FOR,
	AST_IF,
	AST_ELSIF,
	AST_LOOP,
	AST_NULL,
	AST_SWITCH,
	AST_CASE,
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

struct Choice {
	enum {
		CHOICE_NUMEXPR,
		CHOICE_RANGE,
	} type;
	union {
		struct Expression* numExpr;
		struct Range* range;
	} as;
	struct Choice* nextChoice;
};

struct CaseStatement {
	struct AstNode self;

	struct Choice* choices;
	bool defaultCase;
	struct DynamicBlockArray* statements;
};

struct SwitchStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* expression;
	struct DynamicBlockArray* cases;	
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

struct NullStatement {
	struct AstNode self;

	struct Label* label;
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
		NULL_STATEMENT,
		QSIGNAL_ASSIGNMENT,
		SWITCH_STATEMENT,
		VARIABLE_ASSIGNMENT,
		WAIT_STATEMENT,
		WHILE_STATEMENT,
	} type;
	union {
		struct ForStatement forStatement;
		struct IfStatement ifStatement;
		struct LoopStatement loopStatement;
		struct NullStatement nullStatement;
		struct SignalAssign signalAssignment;
		struct SwitchStatement switchStatement;
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
	struct Identifier* name;
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
