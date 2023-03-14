#ifndef INC_LEXER_H
#define INC_LEXER_H

#include "token.h"

void InitLexer(char* in);
Token NextToken();
const char* TokenToString(enum TOKEN_TYPE type);
void PrintToken(Token t);


#endif // INC_LEXER_H
