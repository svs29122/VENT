#ifndef INC_LEXER_H
#define INC_LEXER_H

#include "token.h"

void InitLexer(char* in);
struct Token NextToken();
const char* TokenToString(enum TOKEN_TYPE type);
void PrintToken(struct Token t);


#endif // INC_LEXER_H
