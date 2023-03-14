#ifndef INC_LEXER_H
#define INC_LEXER_H

#include "token.h"

void InitLexer(char* in);
Token NextToken();
void PrintToken(Token t);


#endif // INC_LEXER_H
