#ifndef INC_PARSER_H
#define INC_PARSER_H

struct parser;
struct parser* NewParser(struct lexer*);

Program* ParseProgram(struct parser*);

#endif // INC_PARSER_H
