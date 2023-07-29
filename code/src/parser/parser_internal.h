#ifndef INC_PARSER_INTERNAL_H
#define INC_PARSER_INTERNAL_H

struct parser {
   bool printTokenFlag;
   struct Token currToken;
   struct Token peekToken;
};

extern struct parser* p;

void nextToken();
void freeParserTokens();

//forward declarations needed for parser
static struct PortDecl parsePortDecl();
static struct GenericDecl parseGenericDecl();


#endif // INC_PARSER_INTERNAL_H
