#ifndef INC_PARSER_H
#define INC_PARSER_H

#include <stdbool.h>

extern bool hadError;

void SetPrintTokenFlag();

struct Program* ParseProgram(char* ventProgram);
void FreeProgram(struct Program *prog);


#endif // INC_PARSER_H
