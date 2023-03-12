#ifndef INC_LEXER_H
#define INC_LEXER_H

#include "token.h"

struct lexer;
struct lexer* NewLexer(char* in);

Token NextToken(struct lexer* l);
void PrintToken(Token t);


#endif // INC_LEXER_H
