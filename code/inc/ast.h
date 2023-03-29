#ifndef INC_AST_H
#define INC_AST_H

#include "token.h"
#include "dba.h"

typedef struct Expression Expression;
typedef struct BinaryExpr BinaryExpr;
typedef struct CharExpr CharExpr;
typedef struct NameExpr NameExpr;
typedef struct Identifier Identifier;
typedef struct DataType DataType;
typedef struct Label Label;
typedef struct PortMode PortMode;
typedef struct SignalAssign SignalAssign;
typedef struct SignalDecl SignalDecl;
typedef struct ArchitectureDecl ArchitectureDecl;
typedef struct PortDecl PortDecl;
typedef struct EntityDecl EntityDecl;
typedef struct DesignUnit DesignUnit;
typedef struct UseStatement UseStatement;
typedef struct Program Program;
typedef struct OperationBlock OperationBlock; 
typedef void (*astNodeOpPtr) (void*);

void noOp(void*);
OperationBlock* initOperationBlock(void);
void WalkTree(Program*, OperationBlock*);

struct OperationBlock {
	astNodeOpPtr doProgOp;
	astNodeOpPtr doBlockArrayOp;
	astNodeOpPtr doUseStatementOp;
	astNodeOpPtr doDesignUnitOp;
	astNodeOpPtr doEntityDeclOp;
	astNodeOpPtr doArchDeclOp;
	astNodeOpPtr doPortDeclOp;
	astNodeOpPtr doSignalDeclOp;
	astNodeOpPtr doSignalAssignOp;
	astNodeOpPtr doIdentifierOp;
	astNodeOpPtr doPortModeOp;
	astNodeOpPtr doDataTypeOp;
	astNodeOpPtr doExpressionOp;
};

typedef enum {
		BINARY_EXPR,
		UNARY_EXPR,
		GROUPED_EXPR,
		NAME_EXPR,
		PHYLIT_EXPR,
		CHAR_EXPR,
		STRING_EXPR,
		AGGREGATE_EXPR,
		Q_EXPR,
		NEW_EXPR,
		CALL_EXPR,
} ExpressionType;

struct Expression {
#ifdef DEBUG
	Token token;
#endif
	ExpressionType type;
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

struct Identifier {
	struct Expression self;
	char* value;
};

struct DataType {
#ifdef DEBUG
	Token token;
#endif
	char* value;
};

struct Label {
#ifdef DEBUG
	Token token;
#endif
	char* value;
};

struct PortMode {
#ifdef DEBUG
	Token token;
#endif
	char* value;
};

struct SignalAssign {
#ifdef DEBUG
	Token token; // the "<=" operator
#endif
	struct Label label;
	struct Identifier* target;
	struct Expression* expression;
};

struct SignalDecl {
#ifdef DEBUG
	Token token; // the sig keyword
#endif
	//need to add support for , separated identifier list
	struct Identifier *name;
	struct DataType* dtype;
	struct Expression* expression;
};

struct ArchitectureDecl {
#ifdef DEBUG
	Token token; //the arch keyword
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
	Token token; // the ent keyword
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
	Token token;
#endif
	char* value;
};

struct Program {
	struct DynamicBlockArray* useStatements;
	struct DynamicBlockArray* units;
};

#endif //INC_AST_H
