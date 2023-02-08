#ifndef INC_LEXER_H
#define INC_LEXER_H

struct lexer;

struct lexer* initLexer(FILE* in);
int readChar(struct lexer* lex);
char getChar(struct lexer* lex);


#endif // INC_LEXER_H
