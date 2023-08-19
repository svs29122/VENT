#ifndef INC_EXPRESSION_H
#define INC_EXPRESSION_H

void freeExpression(struct Expression* expr);
struct Expression* createBinaryExpression(struct Expression* l, char* op, struct Expression* r);

#endif //INC_EXPRESSION_H
