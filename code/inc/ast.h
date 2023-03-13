#ifndef INC_AST_H
#define INC_AST_H

#include "token.h"

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

struct DataType {
	Token token;
};

struct Identifier {
	Token token;
	char* value;
};

struct Label {
	Token token;
	char* value;
};

struct PortMode {
	Token token;
	char* value;
};

struct SignalAssignment {
	Token token; // the "<=" operator
	struct Label label;
	struct Identifier target;
	void* expression;
};

struct SignalDecl {
	Token token; // the sig keyword
	struct Identifier *names;
	struct DataType dtype;
	void* expression;
};

struct ArchitectureDecl {
	Token token; //the arch keyword
	struct Identifier archName;
	struct Identifier entName;
	void* declarations;
	void* statements;
};

struct PortDecl {
	struct Identifier *names;
	struct PortMode pmode;
	struct DataType dtype; 
};

struct EntityDecl {
	Token token; // the ent keyword
	struct Identifier* name;
	struct PortDecl* ports;
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
	Token token;
	char* value;
};

struct Program {
	struct UseStatement statements[5];
	struct DesignUnit units[5];
	//TODO: implement variable size versions
	//struct UseStatement* statements;
	//struct DesignUnit* units;
};

#endif //INC_AST_H
