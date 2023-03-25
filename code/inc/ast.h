#ifndef INC_AST_H
#define INC_AST_H

#include "token.h"
#include "dba.h"

typedef struct DataType DataType;
typedef struct Identifier Identifier;
typedef struct Label Label;
typedef struct PortMode PortMode;
typedef struct SignalAssignment SignalAssignment;
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
	astNodeOpPtr doPortDeclOp;
	astNodeOpPtr doIdentifierOp;
	astNodeOpPtr doPortModeOp;
	astNodeOpPtr doDataTypeOp;
};

struct DataType {
#ifdef DEBUG
	Token token;
#endif
	char* value;
};

struct Identifier {
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

struct SignalAssignment {
#ifdef DEBUG
	Token token; // the "<=" operator
#endif
	struct Label label;
	struct Identifier target;
	void* expression;
};

struct SignalDecl {
#ifdef DEBUG
	Token token; // the sig keyword
#endif
	struct Identifier *names;
	struct DataType dtype;
	void* expression;
};

struct ArchitectureDecl {
#ifdef DEBUG
	Token token; //the arch keyword
#endif
	struct Identifier archName;
	struct Identifier entName;
	void* declarations;
	void* statements;
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
	} decl;
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
