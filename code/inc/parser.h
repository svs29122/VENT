#ifndef INC_PARSER_H
#define INC_PARSER_H

void InitParser();
void SetPrintTokenFlag();

struct Program* ParseProgram();
void FreeProgram(struct Program *prog);


#endif // INC_PARSER_H
