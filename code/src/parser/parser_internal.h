#ifndef INC_PARSER_INTERNAL_H
#define INC_PARSER_INTERNAL_H

#include <dht.h>

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


#endif // INC_PARSER_INTERNAL_H
