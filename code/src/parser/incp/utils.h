#ifndef INC_UTILS_H
#define INC_UTILS_H

#include <stdbool.h>

#include "internal_parser.h"

bool match(enum TOKEN_TYPE type);
bool peek(enum TOKEN_TYPE type);

void consume(enum TOKEN_TYPE type, const char* msg);
void consumeNext(enum TOKEN_TYPE type, const char* msg);

struct Token copyToken(struct Token oldToken);
void destroyToken(struct Token thisToken);

bool validDataType();
bool userDefinedDataType();
bool validAssignment();

struct ComponentDecl* getComponentFromStore(char* cname);

bool endOfProgram();
bool thereAreDeclarations();
bool thereAreDesignUnits();
bool thisIsADeclaration();
bool thisIsAPort();
bool thisIsAWildCard(struct Expression* map);
bool thisIsAGenericMap(struct Expression* map, struct Identifier* name, uint16_t pos);

bool positionalMapping(struct Expression* expr);
bool associativeMapping(struct Expression* expr);

#endif // INC_UTILS_H
