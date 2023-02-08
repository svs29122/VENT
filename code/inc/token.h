/*
	token.h
	
	token definitions
	--
*/

typedef enum TOKEN_TYPE {
	
	//punctuation
	LPAREN,
	RPAREN,
	COLON,
	SEMI_COLON,
	COMMA,

	//operators
	ABS,
	XNOR,
	AND,
	MOD,
	NAND,
	NOR,
	NOT,
	OR,
	REM,
	ROL,
	ROR,
	SLA,
	SLL,
	SRA,
	SRL,
	XOR,
	EQUAL,
	NEQUAL,
	LESS_THAN,
	LESS_THAN_EQ,
	VAR_ASSIGN,
	ASSIGN,
	GREATER_THAN,
	GREATER_THAN_EQ,
	ADD,
	SUB,
	CONCAT,
	MULT,
	DIV,
	EXP,

	//data types
	BIT,
	BIT_VECTOR,
	BOOLEAN,
	INTEGER,
	REAL,
	TIME,
	STD_LOGIC,
	STD_LOGIC_VECTOR,
	
	//keywords:
	ALL,
	BEGIN,
	CONST,
	ELSE,
	ELSIF,
	END,
	ENTITY,
	IF,
	IN,
	INOUT,
	IS,
	LIBRARY,
	MAP,
	OF,
	OUT,
	PORT,
	PROCESS,
	RANGE,
	TO,
	SIGNAL,
	USE,
	VARIABLE,
	WHEN,
	
	//literals
	CHAR_LIT,
	STR_LIT,
	BIT_LIT,
	NUM_LIT,
	BASE_LIT,
	PHY_LIT,

	//other
	IDENT,
} ttype;

struct token {
	ttype type;	
	char *literal;
};

