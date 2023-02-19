#ifndef INC_LEXER_H
#define INC_LEXER_H

struct lexer;

struct lexer* NewLexer(char* in);
int readChar(struct lexer* l);
char getChar(struct lexer* l);


#endif // INC_LEXER_H
