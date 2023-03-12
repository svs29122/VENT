#ifndef INC_AST_H
#define INC_AST_H

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

#endif // INC_AST_H
