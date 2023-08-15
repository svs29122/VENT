#ifndef INC_EXPRESSION_H
#define INC_EXPRESSION_H

struct Identifier* copyIdentifier(struct Identifier* orig);
struct Expression* copyExpression(struct Expression* oldExpr);
struct Expression* createBinaryExpression(struct Expression* l, char* op, struct Expression* r);

#endif //INC_EXPRESSION_H
