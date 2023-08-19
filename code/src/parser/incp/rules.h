#ifndef INC_RULES_H
#define INC_RULES_H

typedef struct Expression* (*ParsePrefixFn)();
typedef struct Expression* (*ParseInfixFn)(struct Expression*);

enum Precedence{
   LOWEST_PREC =1,
   LOGICAL_PREC,     // and or xor nand nor xnor
   RELATIONAL_PREC,  // = /= < <= > >=
   SHIFT_PREC,       // sll srl sla sra rol ror
   ADD_PREC,         // + - &
   MULTIPLY_PREC,    // * / mod rem
   PREFIX_PREC,      // ** abs not
   CALL_PREC,        // function(x)
   MAP_PREC,      	// a => b
};

struct ParseRule{
   ParsePrefixFn prefix;
   ParseInfixFn infix;
   enum Precedence precedence;
};

//forward declarations
static struct Expression* parseCall(struct Expression* expr);
static struct Expression* parseAttribute(struct Expression* expr);
static struct Expression* parseBinary(struct Expression* expr);
static struct Expression* parseUnary();
static struct Expression* parseIdentifier();
static struct Expression* parseCharLiteral();
static struct Expression* parseStringLiteral();
static struct Expression* parseNumericLiteral();

static struct ParseRule rules[] = { 
   [TOKEN_IDENTIFIER]   = {parseIdentifier      , NULL            , LOWEST_PREC},
   [TOKEN_CHARLIT]      = {parseCharLiteral     , NULL            , LOWEST_PREC},
   [TOKEN_STRINGLIT]    = {parseStringLiteral   , NULL            , LOWEST_PREC},
   [TOKEN_NUMBERLIT]    = {parseNumericLiteral  , NULL            , LOWEST_PREC},
   [TOKEN_AND]          = {NULL                 , parseBinary     , LOGICAL_PREC},
   [TOKEN_OR]           = {NULL                 , parseBinary     , LOGICAL_PREC},
   [TOKEN_XOR]          = {NULL                 , parseBinary     , LOGICAL_PREC},
   [TOKEN_NOT]          = {parseUnary           , NULL			   , LOGICAL_PREC},
   [TOKEN_EQUAL]        = {NULL                 , parseBinary     , RELATIONAL_PREC},
   [TOKEN_NOT_EQUAL]    = {NULL                 , parseBinary     , RELATIONAL_PREC},
   [TOKEN_GREATER]      = {NULL                 , parseBinary     , RELATIONAL_PREC},
   [TOKEN_GREATER_EQUAL]= {NULL                 , parseBinary     , RELATIONAL_PREC},
   [TOKEN_LESS]         = {NULL                 , parseBinary     , RELATIONAL_PREC},
   [TOKEN_LESS_EQUAL]	= {NULL                 , parseBinary     , RELATIONAL_PREC},
   [TOKEN_PLUS]         = {NULL                 , parseBinary     , ADD_PREC},
   [TOKEN_MINUS]        = {NULL                 , parseBinary     , ADD_PREC},
   [TOKEN_STAR]         = {parseCharLiteral     , parseBinary     , MULTIPLY_PREC},
   [TOKEN_SLASH]        = {NULL                 , parseBinary     , MULTIPLY_PREC},
   [TOKEN_LPAREN]			= {NULL                 , parseCall     	, CALL_PREC},
   [TOKEN_TICK]         = {NULL                 , parseAttribute  , MAP_PREC},
   [TOKEN_MASSIGN]      = {NULL                 , parseBinary     , MAP_PREC},
};

#endif //INC_RULES_H
