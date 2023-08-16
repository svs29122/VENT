#ifndef INC_INTERNAL_PARSER_H
#define INC_INTERNAL_PARSER_H

#include <ast.h>
#include <lexer.h>
#include <token.h>

struct parser {
   bool printTokenFlag;
   struct Token currToken;
   struct Token peekToken;
};

extern struct parser* p;
extern struct DynamicBlockArray* componentStore;

void nextToken();

//forward declarations needed for parser
static struct PortDecl parsePortDecl();
static struct GenericDecl parseGenericDecl();


#endif // INC_INTERNAL_PARSER_H
