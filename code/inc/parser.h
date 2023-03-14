#ifndef INC_PARSER_H
#define INC_PARSER_H

#include <ast.h>

void InitParser();
Program* ParseProgram();
void PrintProgram(Program*);

#endif // INC_PARSER_H
