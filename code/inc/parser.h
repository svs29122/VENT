#ifndef INC_PARSER_H
#define INC_PARSER_H

void SetPrintTokenFlag();

struct Program* ParseProgram(char* ventProgram);
void FreeProgram(struct Program *prog);


#endif // INC_PARSER_H
