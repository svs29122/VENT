#ifndef INC_UTILS_H
#define INC_UTILS_H

#include <stdbool.h>

#include "parser_internal.h"

bool match(enum TOKEN_TYPE type);
bool peek(enum TOKEN_TYPE type);

void consume(enum TOKEN_TYPE type, const char* msg);
void consumeNext(enum TOKEN_TYPE type, const char* msg);

bool validDataType();
bool validAssignment();

bool thereAreDeclarations();

#endif // INC_UTILS_H
