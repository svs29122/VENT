#ifndef INC_TOKEN_H
#define INC_TOKEN_H

enum TOKEN_TYPE {
	
	//delmiters
	LPAREN = 1,
	RPAREN, 
	COLON, 
	SCOLON, 
	LBRACE, 			//5 
	RBRACE, 
	COMMA, 
	TICK,

	//operators
	SLASH, 
	STAR, 			//10 
	MINUS,
	PLUS, 
	EQUAL, 
	NOT_EQUAL, 
	GREATER, 		//15 
	GREATER_EQUAL,
	LESS, 
	LESS_EQUAL, 
	AND, 
	OR, 				//20
	XOR, 
	NOT,

	//assignment and declaration
	INPUT, 
	OUTPUT, 
	INOUT,			//25 
	SASSIGN, 
	VASSIGN, 
	AASSIGN,

	//types
	STL, 
	STLV, 			//30
	SIG, 
	VAR, 
	INTEGER, 
	STRING, 
	BIT,				//35 
	BITV,
	SIGNED,
	UNSIGNED,

	//literals
	IDENTIFIER, 
	CHARLIT,
	NUMBERLIT,
	STRINGLIT,		
	BSTRINGLIT,

	//keywords
	ENT, 
	ARCH, 			
	GEN, 
	COMP, 			
	MAP, 
	PROC,
	OTHER,			
	IF, 
	ELSIF, 			
	ELSE, 
	FOR, 
	USE,
	WHILE,			 
	WAIT,

	ILLEGAL,			
};

typedef struct token {
	enum TOKEN_TYPE type;	
	char *literal;
} Token;

const char* tokenToString(enum TOKEN_TYPE type);

enum VHD_TOKEN {
	
	//punctuation
	VHD_LPAREN,
	VHD_RPAREN,
	VHD_COLON,
	VHD_SEMI_COLON,
	VHD_COMMA,

	//operators
	VHD_ABS,
	VHD_XNOR,
	VHD_AND,
	VHD_MOD,
	VHD_NAND,
	VHD_NOR,
	VHD_NOT,
	VHD_OR,
	VHD_REM,
	VHD_ROL,
	VHD_ROR,
	VHD_SLA,
	VHD_SLL,
	VHD_SRA,
	VHD_SRL,
	VHD_XOR,
	VHD_EQUAL,
	VHD_NEQUAL,
	VHD_LESS_THAN,
	VHD_LESS_THAN_EQ,
	VHD_VAR_ASSIGN,
	VHD_ASSIGN,
	VHD_GREATER_THAN,
	VHD_GREATER_THAN_EQ,
	VHD_ADD,
	VHD_SUB,
	VHD_CONCAT,
	VHD_MULT,
	VHD_DIV,
	VHD_EXP,

	//data types
	VHD_BIT,
	VHD_BIT_VECTOR,
	VHD_BOOLEAN,
	VHD_INTEGER,
	VHD_REAL,
	VHD_TIME,
	VHD_STD_LOGIC,
	VHD_STD_LOGIC_VECTOR,
	
	//keywords:
	VHD_ALL,
	VHD_BEGIN,
	VHD_CONST,
	VHD_ELSE,
	VHD_ELSIF,
	VHD_END,
	VHD_ENTITY,
	VHD_IF,
	VHD_IN,
	VHD_INOUT,
	VHD_IS,
	VHD_LIBRARY,
	VHD_MAP,
	VHD_OF,
	VHD_OUT,
	VHD_PORT,
	VHD_PROCESS,
	VHD_RANGE,
	VHD_TO,
	VHD_SIGNAL,
	VHD_USE,
	VHD_VARIABLE,
	VHD_WHEN,
	
	//literals
	VHD_CHAR_LIT,
	VHD_STR_LIT,
	VHD_BIT_LIT,
	VHD_NUM_LIT,
	VHD_BASE_LIT,
	VHD_PHY_LIT,

	//other
	VHD_IDENT,
};

struct revToken {
	enum VHD_TOKEN type;	
	char *literal;
};

#endif // INC_TOKEN_H
