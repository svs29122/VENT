#ifndef INC_PARSER_H
#define INC_PARSER_H

#include "ast.h"
#include "dba.h"

void InitParser();

Program* ParseProgram();

void FreeProgram(Program *prog);
void PrintProgram(Program*);

#endif // INC_PARSER_H
