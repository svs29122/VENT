/*
	ast.c

	this file contains all code relating to the abstract syntax tree 
	used to represent a VENT file in memory. This tree is constructed
	by the parser using a list of tokens.
	--
*/

#include "token.h"

typedef struct datatype {
	Token token;
} DataType;

typedef struct identifier {
	Token token;
	char* value;
} Identifier;

typedef struct signaldecl {
	Token token;
	Identifier *names;
	DataType type;
	char* intializer;
} SignalDecl;

typedef struct program {
	void* statements;
} Program;
