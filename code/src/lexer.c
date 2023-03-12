/*
	lexer.c

	This file contains all code relating to transforming (lexing) 
	a VENT file into a series of tokens that the parser can process 
	--
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lexer.h"

static char readChar(struct lexer* l);

struct lexer {
	char *input;
	char ch;

	int currPos;
	int readPos;
	int line;
};

struct lexer* NewLexer(char* in){
	struct lexer* newLexer = (struct lexer*)malloc(sizeof(struct lexer));

	newLexer->input = in;
	newLexer->currPos = -1;
	newLexer->readPos = 0;
	newLexer->line = 1;

	//init our lexer with a char
	readChar(newLexer);

	return newLexer;
}

static Token newToken(enum TOKEN_TYPE type, char literal){
	Token tok;

	tok.type = type;
	tok.literal = (char*)malloc(sizeof(char) + 1);
	tok.literal[0] = literal;
	tok.literal[1] = '\0';

	return tok;
}

static Token newMultiCharToken(enum TOKEN_TYPE type, struct lexer* l, int len){
	Token tok = {ILLEGAL, 0};

	//we've already passed the first char of token  so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	for(int i = 1; i < len-1; i++){
		readChar(l);
		end++;
	}	

	//move the lexer past the token
	readChar(l);

	int lSize = sizeof(char) * (int)(end-start) + 1;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize] = '\0';

	tok.type = type;

	return tok;
}

static char peek(struct lexer* l){
	return l->input[l->currPos];
}

static char peekNext(struct lexer* l){
	return l->input[l->readPos];
}

// 'get' utilities
static enum TOKEN_TYPE checkKeyword(int start, int keyLen, int litLen, const char* lit, const char* rest, enum TOKEN_TYPE type){
	
	if(litLen == start + keyLen && memcmp(&lit[start], rest, keyLen) == 0){
		return type;
	}

	return IDENTIFIER;
}

static enum TOKEN_TYPE getIdentifierType(int size, char* lit){
	
	switch(lit[0]){
		case 'a':
			if(size > 1){
				switch(lit[1]){
					case 'r':	return checkKeyword(2, 2, size, lit, "ch", ARCH);
					case 'n':	return checkKeyword(2, 1, size, lit, "d", AND);
				}
			}
			break;
		case 'e': return checkKeyword(1, 2, size, lit, "nt", ENT);
		case 'i': return checkKeyword(1, 2, size, lit, "nt", INTEGER);
		case 's': 
			if(size > 1){
				switch(lit[1]){
					case 'i':
						if(size > 2){
							switch(lit[2]){
								case 'g':
									if(size > 3) return checkKeyword(3, 3, size, lit, "ned", SIGNED);
									else return SIG;
							}
						}
						break;
					case 't':
						if(size > 2){
							switch(lit[2]){
								case 'l':
									if(size > 3) return checkKeyword(3, 1, size, lit, "v", STLV);
									else return STL;
								case 'r': 
									if(size > 3) return checkKeyword(3, 3, size, lit, "ing", STRING); 
							}
						}
						break;
				}
			}
			break;
		case 'u':
			if(size > 1){
				switch(lit[1]){
					case 's':
						if(size > 2) return checkKeyword(2, 1, size, lit, "e", USE);
						break;
					case 'n':
						if(size > 2) return checkKeyword(2, 6, size, lit, "signed", UNSIGNED);
						break;
				}
			}
			break;
	}

	return IDENTIFIER;
}

// 'read' utilities
static char readChar(struct lexer* l){
	
	//grab next char in buffer
	l->ch = l->input[l->readPos];
	
	//bump positions	
	l->currPos = l->readPos;
	l->readPos += 1;

	return l->ch;
}

static Token readIdentifier(struct lexer *l){
	Token tok = {ILLEGAL, 0};

	//we've already passed the first char of
	//identifier  so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	//check for any chars that can follow an identifier
	//TODO: this is gonna be huge, maybe break it out
	//to a separate function or a few different functions?
	
	if(peek(l) != ' ' && peek(l) != ';') {	
		while(peekNext(l) != ' ' && peekNext(l) != '=' &&
				peekNext(l) != '{' && peekNext(l) != '}' &&
				peekNext(l) != '(' && peekNext(l) != ')' &&
				peekNext(l) != '<' && peekNext(l) != '>' &&
				peekNext(l) != '+' && peekNext(l) != '-' &&
				peekNext(l) != '*' && peekNext(l) != '/' &&
				peekNext(l) != ';' && peekNext(l) != ',' &&
				peekNext(l) != '\n' && peekNext(l) != '\0'){
	
			readChar(l);
			end++;
		}
	} else {
		//got a single letter identifier
		end--;
	}

	//step the lexer past the identifier
	if(start != end) readChar(l);	

	int lSize = sizeof(char) * (int)(end-start) + 1;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize] = '\0';

#ifdef DEBUG 
	printf("DEBUG: identifer == %s\r\n", tok.literal); 
#endif
	tok.type = getIdentifierType(lSize, tok.literal);

	return tok;
}

static Token readStringLiteral(struct lexer *l){
	Token tok = {ILLEGAL, 0};

	//we've already passed the first " of the string
	// so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	while(peek(l) != '"' && peek(l) != '\0'){
		readChar(l);
		end++;
	}	

	//add the end quote too
	if(peek(l) != '\0'){
		readChar(l);
		end++;
	}

	tok.type = STRINGLIT;
	
	int lSize = sizeof(char) * (int)(end-start) + 1;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize] = '\0';

	return tok;
}

static Token readBitStringLiteral(struct lexer *l){
	Token tok = {ILLEGAL, 0};

	//we've already passed the first base char of 
	//the bitstring so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	//add the start quote
	if(peek(l) != '\0'){
		readChar(l);
		end++;
	}

	while(peek(l) != '"' && peek(l) != '\0'){
		readChar(l);
		end++;
	}	

	//add the end quote too
	if(peek(l) != '\0'){
		readChar(l);
		end++;
	}

	tok.type = BSTRINGLIT;
	
	int lSize = sizeof(char) * (int)(end-start) + 1;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize] = '\0';

	return tok;
}

static Token readNumericLiteral(struct lexer *l){
	Token tok = {ILLEGAL, 0};

	//we've already passed the first char of number so start at currPos-1
	char *start = &(l->input[l->currPos-1]);
	char *end = start+1;

	//TODO: assuming a space following the number?
	// no way that's correct
	while(peek(l) != ' ' && peek(l) != '\0'){
		readChar(l);
		end++;
	}	

	tok.type = NUMBERLIT;
		
	int lSize = sizeof(char) * (int)(end-start) + 1;
	tok.literal = (char*)malloc(lSize);
	strncpy(tok.literal, start, lSize);
	tok.literal[lSize] = '\0';

	return tok;
}

static Token readCharLiteral(struct lexer *l){
	
	char literal = peek(l);
	Token tok = newToken(CHARLIT, literal);		
	
	//move lexer past literal and '
	readChar(l);
	readChar(l);

	return tok;
}

// 'is' utilities
static bool isCharLiteral(struct lexer *l) {
   return peekNext(l) == '\'';
}

static bool isBitStringLiteral(struct lexer *l) {
   return peek(l) == '"';
}

static bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool isNumber(char c) {
    return c >= '0' && c <= '9';
}

static void skipWhiteSpace(struct lexer *l){
	for(;;){
		switch(l->ch){
			case ' ':
			case '\t':
			case '\r':
				readChar(l);
				break;

			case '\n':
				l->line++;
				readChar(l);
				break;	

			case '/':
				if(peekNext(l) == '/') {
					// handle single-line comment
					while(peek(l) != '\n' && peek(l) != '\0'){
						 readChar(l);
					}
				} else if (peekNext(l) == '*'){
					//handle multi-line comment
					readChar(l);
					readChar(l);
					while((peek(l) != '*' || peekNext(l) != '/') && peek(l) != '\0'){
						readChar(l);
					}
					readChar(l);
					readChar(l);
				} else {
					return;
				}
				break;

			 default:
				return;
		}
	}
}

Token NextToken(struct lexer* l) {
	
	skipWhiteSpace(l);

	char ch = l->ch;
	readChar(l);

	switch(ch){
		case '(' : return newToken(LPAREN, ch);
		case ')' : return newToken(RPAREN, ch);
		case ':' : 
			if(peek(l) == '=') return newMultiCharToken(VASSIGN, l, 2);
			return newToken(COLON, ch);
		case ';' : return newToken(SCOLON, ch);
		case '{' : return newToken(LBRACE, ch);
		case '}' : return newToken(RBRACE, ch);
		case ',' : return newToken(COMMA, ch);
		case '/' : return newToken(SLASH, ch);
		case '*' : return newToken(STAR, ch);
		case '-' :
			if(peek(l) == '>') return newMultiCharToken(INPUT, l, 2); 			
			return newToken(MINUS, ch);
		case '<' :
			if(peek(l) == '-') {
				if(peekNext(l) == '>') return newMultiCharToken(INOUT, l, 3);
				return newMultiCharToken(OUTPUT, l, 2);
			} else if(peek(l) == '='){
				return newMultiCharToken(SASSIGN, l, 2);
			} 
			return newToken(LESS, ch);
		case '+' : return newToken(PLUS, ch);
		case '=' : 
			if(peek(l) == '>') return newMultiCharToken(AASSIGN, l, 2); 			
			return newToken(EQUAL, ch);
		case '\0': return newToken(EOP, ch);
		case '\'':
			if(isCharLiteral(l)) return readCharLiteral(l);
			return newToken(TICK, ch);
		case '"': return readStringLiteral(l);
		case 'B':
		case 'O':
		case 'X':
			if(isBitStringLiteral(l)){
				return readBitStringLiteral(l);
			} // else must be identifier. Falling through to default!!! 
		default:
			if(isLetter(ch)){
				return readIdentifier(l); 
			} else if (isNumber(ch)){
				return readNumericLiteral(l);
			} else {
				return newToken(ILLEGAL, ch);
			}
	}
}

const char* tokenToString(enum TOKEN_TYPE type){
	switch(type){
		case LPAREN: 		return "LPAREN";
		case RPAREN:		return "RPAREN";
		case COLON:			return "COLON";
		case SCOLON: 		return "SCOLON";
		case LBRACE:		return "LBRACE";
		case RBRACE:		return "RBRACE";
		case COMMA:			return "COMMA";
		case TICK:			return "TICK";
		case SLASH:			return "SLASH";
		case STAR:			return "STAR";
		case MINUS: 		return "MINUS";
		case PLUS:	 		return "PLUS";
		case EQUAL: 		return "EQUAL";
		case NOT_EQUAL:	return "NOT_EQUAL"; 
		case GREATER: 		return "GREATER";
		case LESS:	 		return "LESS";
		case AND: 			return "AND";
		case OR: 			return "OR";
		case XOR: 			return "XOR";
		case NOT: 			return "NOT";
		case INPUT: 		return "INPUT";
		case OUTPUT: 		return "OUTPUT";
		case INOUT: 		return "INOUT";
		case SASSIGN: 		return "SASSIGN";
		case VASSIGN: 		return "VASSIGN";
		case AASSIGN: 		return "AASSIGN";
		case STL:	 		return "STL";
		case STLV:	 		return "STLV";
		case SIG:	 		return "SIG";
		case VAR:	 		return "VAR";
		case INTEGER: 		return "INTEGER";
		case STRING: 		return "STRING";
		case BIT:	 		return "BIT";
		case BITV: 			return "BITV";
		case SIGNED: 		return "SIGNED";
		case UNSIGNED: 	return "UNSIGNED";
		case IDENTIFIER:	return "IDENTIFIER";
		case CHARLIT: 		return "CHARLIT";
		case STRINGLIT:	return "STRINGLIT";
		case BSTRINGLIT:	return "BSTRINGLIT";
		case ENT:	 		return "ENT";
		case ARCH:	 		return "ARCH";
		case GEN:	 		return "GEN";
		case COMP:	 		return "COMP";
		case MAP:	 		return "MAP";
		case PROC: 			return "PROC";
		case OTHER: 		return "OTHER";
		case IF: 			return "IF";
		case ELSIF: 		return "ELSIF";
		case FOR:	 		return "FOR";
		case USE:	 		return "USE";
		case WHILE: 		return "WHILE";
		case WAIT:	 		return "WAIT";
		case EOP:	 		return "EOP";
		case ILLEGAL: 		return "ILLEGAL";
		default: 			return "";
	}
}

void PrintToken(Token t){
	printf("type: %10s, literal: %s\n", tokenToString(t.type), t.literal);
}
