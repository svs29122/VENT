#ifndef INC_INTERNAL_PARSER_H
#define INC_INTERNAL_PARSER_H

#include <ast.h>
#include <lexer.h>
#include <token.h>
#include <dht.h>

struct parser {
   bool printTokenFlag;
   struct Token currToken;
   struct Token peekToken;
};

extern struct parser* p;
extern struct DynamicBlockArray* componentStore;
extern struct DynamicHashTable* enumTypeTable;

void nextToken();

//forward declarations needed for parser
static struct PortDecl parsePortDecl();
static struct GenericDecl parseGenericDecl();


#endif // INC_INTERNAL_PARSER_H
