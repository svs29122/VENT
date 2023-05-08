#ifndef INC_EXPRESSION_H
#define INC_EXPRESSION_H

enum Precedence{
	LOWEST_PREC =1,
	LOGICAL_PREC, 		// and or xor nand nor xnor
	RELATIONAL_PREC, 	// = /= < <= > >=
	SHIFT_PREC, 		// sll srl sla sra rol ror
	ADD_PREC, 			// + - &
	MULTIPLY_PREC, 	// * / mod rem
	PREFIX_PREC, 		// ** abs not
	CALL_PREC, 			// function(x)
};

typedef struct Expression* (*ParsePrefixFn)();
typedef struct Expression* (*ParseInfixFn)(struct Expression*);

struct ParseRule{
	ParsePrefixFn prefix;
	ParseInfixFn infix;
	enum Precedence precedence;
};

//forward declarations
static struct Expression* parseBinary(struct Expression* expr);
static struct Expression* parseIdentifier();
static struct Expression* parseCharLiteral();
static struct Expression* parseNumericLiteral();

struct ParseRule rules[] = {
	[TOKEN_IDENTIFIER] 	= {parseIdentifier		, NULL				, LOWEST_PREC},
	[TOKEN_CHARLIT] 		= {parseCharLiteral		, NULL				, LOWEST_PREC},
	[TOKEN_NUMBERLIT] 	= {parseNumericLiteral	, NULL				, LOWEST_PREC},
	[TOKEN_AND] 			= {NULL						, parseBinary		, LOGICAL_PREC},
	[TOKEN_OR]	 			= {NULL						, parseBinary		, LOGICAL_PREC},
	[TOKEN_XOR] 			= {NULL						, parseBinary		, LOGICAL_PREC},
	[TOKEN_EQUAL] 			= {NULL						, parseBinary		, RELATIONAL_PREC},
	[TOKEN_NOT_EQUAL] 	= {NULL						, parseBinary		, RELATIONAL_PREC},
	[TOKEN_GREATER]		= {NULL						, parseBinary		, RELATIONAL_PREC},
	[TOKEN_GREATER_EQUAL]= {NULL						, parseBinary		, RELATIONAL_PREC},
	[TOKEN_LESS] 			= {NULL						, parseBinary		, RELATIONAL_PREC},
	[TOKEN_LESS_EQUAL]	= {NULL						, parseBinary		, RELATIONAL_PREC},
	[TOKEN_PLUS] 			= {NULL						, parseBinary		, ADD_PREC},
	[TOKEN_MINUS] 			= {NULL						, parseBinary		, ADD_PREC},
	[TOKEN_STAR] 			= {NULL						, parseBinary		, MULTIPLY_PREC},
	[TOKEN_SLASH] 			= {NULL						, parseBinary		, MULTIPLY_PREC},
};

static struct ParseRule* getRule(enum TOKEN_TYPE type){
	return &rules[type];
}

#endif //INC_EXPRESSION_H
