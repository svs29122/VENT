#ifndef INC_TOKEN_H
#define INC_TOKEN_H

enum TOKEN_TYPE {
	
	//delmiters
	TOKEN_LPAREN = 1,
	TOKEN_RPAREN, 
	TOKEN_COLON, 
	TOKEN_SCOLON, 
	TOKEN_LBRACE, 			//5 
	TOKEN_RBRACE, 
	TOKEN_COMMA, 
	TOKEN_TICK,
	TOKEN_TO,
	TOKEN_DOWNTO,
	TOKEN_BAR,

	//attributes
	TOKEN_UP,
	TOKEN_DOWN,

	//operators
	TOKEN_SLASH, 
	TOKEN_SLASH_EQUAL, 
	TOKEN_STAR, 			 
	TOKEN_STAR_EQUAL, 			 
	TOKEN_MINUS,
	TOKEN_MINUS_EQUAL,
	TOKEN_MINUS_MINUS,
	TOKEN_PLUS, 
	TOKEN_PLUS_EQUAL, 
	TOKEN_PLUS_PLUS, 
	TOKEN_EQUAL, 
	TOKEN_NOT_EQUAL, 
	TOKEN_GREATER, 		//15 
	TOKEN_GREATER_EQUAL,
	TOKEN_LESS, 
	TOKEN_LESS_EQUAL, 
	TOKEN_AND, 
	TOKEN_OR, 				//20
	TOKEN_XOR, 
	TOKEN_NOT,

	//port direction modes
	TOKEN_INPUT, 
	TOKEN_OUTPUT, 
	TOKEN_INOUT,			//25 

	//assignment
	TOKEN_SASSIGN, //sig assign <=
	TOKEN_VASSIGN, //var assign :=
	TOKEN_MASSIGN, //map assign =>
	
	//declarations
	TOKEN_SIG, 
	TOKEN_VAR, 
	TOKEN_FILE,
	TOKEN_COMP, 			

	//types
	TOKEN_STL, 
	TOKEN_STLV, 			
	TOKEN_INTEGER, 
	TOKEN_STRING, 
	TOKEN_BIT,				 
	TOKEN_BITV,
	TOKEN_SIGNED,
	TOKEN_UNSIGNED,
	TOKEN_TYPE,

	//literals
	TOKEN_IDENTIFIER, 
	TOKEN_CHARLIT,
	TOKEN_NUMBERLIT,
	TOKEN_STRINGLIT,		
	TOKEN_BSTRINGLIT,

	//keywords
	TOKEN_ENT, 
	TOKEN_ARCH, 			
	TOKEN_GEN, 
	TOKEN_MAP, 
	TOKEN_PROC,
	TOKEN_OTHER,			
	TOKEN_CASE,
	TOKEN_DEFAULT,
	TOKEN_SWITCH,
	TOKEN_IF, 
	TOKEN_ELSIF, 			
	TOKEN_ELSE, 
	TOKEN_FOR, 
	TOKEN_USE,
	TOKEN_WHILE,			 
	TOKEN_LOOP,
	TOKEN_WAIT,

	//for simulation
	TOKEN_ASSERT,
	TOKEN_REPORT,
	TOKEN_NOTE,
	TOKEN_WARNING,
	TOKEN_ERROR,
	TOKEN_FAILURE,
	TOKEN_SEVERITY,

	TOKEN_NULL,
	TOKEN_EOP, // end of program			
	TOKEN_ILLEGAL,			
};

struct Token {
	enum TOKEN_TYPE type;	
	int lineNumber;
	char *literal;
};

#endif // INC_TOKEN_H
