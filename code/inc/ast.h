#ifndef INC_AST_H
#define INC_AST_H

#include "token.h"

typedef enum unittype {
	UNIT_ENTITY,
	UNIT_ARCHITECTURE,
	UNIT_PACKAGE,
	UNIT_PACKAGe_BODY,
	UNIT_CONFIGURATION
} UnitType;

typedef struct datatype {
	Token token;
} DataType;

typedef struct identifier {
	Token token;
	char* value;
} Identifier;

typedef struct label {
	Token token;
	char* value;
} Label;

typedef struct portMode {
	Token token;
	char* value;
} PortMode;

typedef struct signalassign{
	Token token; // the "<=" operator
	Label label;
	Identifier target;
	void* expression;
} SignalAssignment;

typedef struct signaldecl {
	Token token; // the sig keyword
	Identifier *names;
	DataType dtype;
	void* expression;
} SignalDecl;

typedef struct archdecl{
	Token token; //the arch keyword
	Identifier archName;
	Identifier entName;
	void* declarations;
	void* statements;
} ArchitectureDecl;

typedef struct portdecl {
	Identifier *names;
	PortMode pmode;
	DataType dtype; 
} PortDecl;

typedef struct entitydecl {
	Token token; // the ent keyword
	Identifier* name;
	PortDecl* ports;
} EntityDecl;

typedef struct designunit {
	UnitType type;
	void* unitDeclaration;
} DesignUnit;

typedef struct usestatement {
	Token token;
	char* value;
} UseStatement;

typedef struct program {
	UseStatement* statements;
	DesignUnit* units;
} Program;

#endif // INC_AST_H
