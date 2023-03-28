/*
	parser.c

	This file contains all code used to parse the list of tokens 
	produced by the lexer. The end result is a syntax tree 
	representing a VENT program/file.
	--
*/

////////////////////////////////////////////////////////////////
/*
	The VENT Grammar
	-----------------

	program 			->			useStmt* designUnit+ EOF;

	useStmt			->			"use" IDENTIFIER  ("." IDENTIFIER)* (".all")?  ;

	designUnit		->			entDecl
									| archDecl
									| pkgDecl
									| pkgbodyDecl
									| cfgDecl;
	
	entDecl			->			"ent" IDENTIFIER "{" portDecl "}" ;

	archDecl			->			"arch" IDENTIFIER "(" IDENTIFIER ")" "{" declaration* statement* "}" ;

	portDecl			->			(IDENTIFIER ("," IDENTIFIER)* portMode dataType ";" )* ;

	portMode	   	->			"->" | "<-" | "<->" | ">-<" ;

	dataType			->			"stl" | ("stlv" "(" INTEGER ":" INTEGER ")") ;

	declaration		->			prodDecl
									| prodBody
									| funDecl
									| funBody
									| typeDecl
									| subTypeDecl
									| constDecl
									| sigDecl
									| varDecl
									| fileDecl
									| compDecl
									| configSpec
									| aliasDecl
									| attrDecl
									| useStmt ;

	sigDecl			->			"sig" IDENTIFIER ("," IDENTIFIER)* dataType (":=" expression)? ";" ;

	statement		->			process
									| instantiation 
									| signalAssign
									| generate
									| assert
									| procedureCall
									| block

	signalAssign	->			(LABEL ":")? IDENTIFIER "<=" expression ";" ; 
	
	expression 		->			expression binaryOp expression
									| unaryOp expression
									| (expression)
									| name
									| number
									| phyLiteral
									| CHAR
									| STRING 
									| aggregate
									| qExpression
									| new
									| functionCall

	binaryOp			->			"+" | "-" | "*" | "/" | "mod" | "rem" | "**"
									| "=" | "/=" | "<" | "<=" | ">" | ">=" | "&"
									| "and" | "or" | "xor" | "nand" | "nor" | "xnor"
									| "sll" | "srl" | "sla" | "sra" | "rol" | "ror" 
	
	unaryOp			->			"+" | "-" | "abs" | "not" 

	name				->			IDENTIFIER

*/
////////////////////////////////////////////////////////////////
/*
	E.g. production:
		
		tempSig <= a and b;

		sigAssign 
		-> IDENTIFIER "<=" expression ";" 
		-> IDENTIFIER "<=" expression binaryOp expression ";"
		-> IDENTIFIER "<=" name "and" name ";"
		-> IDENTIFIER "<=" IDENTIFIER "and" IDENTIFIER ";"
		-> "tempSig" "<=" "a" "and" "b" ";"
		-> "tempSig <= a and b;"
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"

typedef enum {
	LOWEST_PREC =1,
	LOGICAL_PREC, 		// and or xor nand nor xnor
	RELATIONAL_PREC, 	// = /= < <= > >=
	SHIFT_PREC, 		// sll srl sla sra rol ror
	ADD_PREC, 			// + - &
	MULTIPLY_PREC, 	// * / mod rem
	PREFIX_PREC, 		// ** abs not
	CALL_PREC, 			// function(x)
} Precedence;

typedef Expression* (*ParseFn)();

typedef struct {
	ParseFn prefix;
	ParseFn infix;
	Precedence precedence;
} ParseRule;

//forward declarations
static Expression* parseIdentifier();
static Expression* parseCharlit();

ParseRule rules[] = {
	[TOKEN_IDENTIFIER] = {parseIdentifier, NULL, LOWEST_PREC},
	[TOKEN_CHARLIT] = {parseCharlit, NULL, LOWEST_PREC},
};

static ParseRule* getRule(enum TOKEN_TYPE type){
	return &rules[type];
}

struct parser {
	Token currToken;
	Token peekToken;
} static parser;

static struct parser *p = &parser;

void InitParser(){
	
	memset(p, 0, sizeof(struct parser));

	p->currToken = NextToken();
	p->peekToken = NextToken();	
}

static Token nextToken(){	
	
	free(p->currToken.literal);

	p->currToken = p->peekToken;
	p->peekToken = NextToken();
}

static bool match( enum TOKEN_TYPE type){
	return p->currToken.type == type;
}

static bool peek( enum TOKEN_TYPE type){
	return p->peekToken.type == type;
}

static bool validDataType(){
	bool valid = false; 
	
	valid = 	match(TOKEN_STL) 		||	match(TOKEN_STLV) 	||
				match(TOKEN_INTEGER) || match(TOKEN_STRING) 	||
				match(TOKEN_BIT) 		|| match(TOKEN_BITV)		||
				match(TOKEN_SIGNED)	|| match(TOKEN_UNSIGNED);

	return valid;
}

static Expression* parseExpression(void){
	ParseFn prefixRule = getRule(p->currToken.type)->prefix;
	if(prefixRule == NULL){
		printf("Error: expected expression tok type %d\n", p->currToken.type);
		return NULL;
	}	
	Expression* leftExp = prefixRule();

	if(peek(TOKEN_SCOLON)){
		nextToken();
	}

	return leftExp;
}

static Expression* parseIdentifier(){
	Identifier* ident = calloc(1, sizeof(Identifier));
#ifdef DEBUG
	memcpy(&(ident->self.token), &(p->currToken), sizeof(Token));
#endif

	ident->self.type = NAME_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	ident->value = calloc(size, sizeof(char));
	memcpy(ident->value, p->currToken.literal, size);
	
	return &(ident->self);
}

static Expression* parseCharlit(){
	CharExpr* chexp = calloc(1, sizeof(CharExpr));
#ifdef DEBUG
	memcpy(&(chexp->self.token), &(p->currToken), sizeof(Token));
#endif

	chexp->self.type = CHAR_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	chexp->literal = calloc(size, sizeof(char));
	memcpy(chexp->literal, p->currToken.literal, size);
	
	return &(chexp->self);
}

static Expression* parseBinary(){
	BinaryExpr* biexp = calloc(1, sizeof(BinaryExpr));
#ifdef DEBUG
	memcpy(&(biexp->self.token), &(p->currToken), sizeof(Token));
#endif

	biexp->self.type = BINARY_EXPR;

	//need to store operator and get left/right epressions

	return &(biexp->self);
}

static DataType* parseDataType(char* val){
	DataType* dt = calloc(1, sizeof(DataType));
#ifdef DEBUG
	memcpy(&(dt->token), &(p->currToken), sizeof(Token));
#endif

	int size = strlen(val) + 1;
	dt->value = calloc(size, sizeof(char));
	memcpy(dt->value, val, size);

	return dt;
}

static PortMode* parsePortMode(char* val){
	PortMode* pm = calloc(1, sizeof(PortMode));
#ifdef DEBUG
	memcpy(&(pm->token), &(p->currToken), sizeof(Token));
#endif

	int size = strlen(val) +1;
	pm->value = calloc(size, sizeof(char));
	memcpy(pm->value, val, size);

	return pm;
}

static Dba* parseArchBodyStatements(){
	Dba* stmts = initBlockArray(sizeof(SignalAssign));
	
	while(!match(TOKEN_RBRACE)){
		SignalAssign* stmt = calloc(1, sizeof(SignalAssign));

		if(!match(TOKEN_IDENTIFIER)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}
		stmt->target = (Identifier*)parseIdentifier();

		nextToken();
		if(!match(TOKEN_SASSIGN)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}
		
		nextToken();
		stmt->expression = parseExpression();

		if(!match(TOKEN_SCOLON)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
			printf("Expected token: %d, but got %d\r\n", TOKEN_SCOLON, p->currToken.type);
		}	
		writeBlockArray(stmts, (char*)stmt);
		free(stmt);
	
		nextToken();	
	}

	return stmts;
}

static Dba* parseArchBodyDeclarations(){
	Dba* decls = initBlockArray(sizeof(SignalDecl));

	while(match(TOKEN_SIG)){
		SignalDecl* decl = calloc(1, sizeof(SignalDecl));

		nextToken();
		if(!match(TOKEN_IDENTIFIER)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		decl->name = (Identifier*)parseIdentifier();
		
		nextToken();
		if(!validDataType()){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		decl->dtype = parseDataType(p->currToken.literal);
		
		nextToken();
		if(match(TOKEN_VASSIGN)){
			nextToken();
			decl->expression = parseExpression();	
		}

		if(!match(TOKEN_SCOLON)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		writeBlockArray(decls, (char*)decl);
		free(decl);
	
		nextToken();	
	}
	
	return decls;
}

static Dba* parsePortDecl(){
	Dba* ports = initBlockArray(sizeof(PortDecl)); 	

	nextToken();

	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		PortDecl* port = malloc(sizeof(PortDecl));		

		if(!match(TOKEN_IDENTIFIER)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		port->name = (Identifier*)parseIdentifier();
		
		nextToken();
		if(!match(TOKEN_INPUT) && !match(TOKEN_OUTPUT) && !match(TOKEN_INOUT)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		port->pmode = parsePortMode(p->currToken.literal);
		
		nextToken();
		if(!validDataType()){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		port->dtype = parseDataType(p->currToken.literal);
		
		nextToken();
		if(!match(TOKEN_SCOLON)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		writeBlockArray(ports, (char*)port);
		free(port);
		
		nextToken();
	}

	return ports;
}

static void parseArchitectureDecl(ArchitectureDecl* aDecl){
#ifdef DEBUG
	memcpy(&(aDecl->token), &(p->currToken), sizeof(Token));
#endif
	
	nextToken();	
	if(!match(TOKEN_IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	aDecl->archName = (Identifier*)parseIdentifier();
	
	nextToken();
	if(!match(TOKEN_LPAREN)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	nextToken();	
	if(!match(TOKEN_IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	aDecl->entName = (Identifier*)parseIdentifier();
	
	nextToken();
	if(!match(TOKEN_RPAREN)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	nextToken();
	if(!match(TOKEN_LBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	nextToken();
	if(!match(TOKEN_RBRACE)){
		aDecl->declarations = parseArchBodyDeclarations();	
		aDecl->statements = parseArchBodyStatements();	
	}

	if(!match(TOKEN_RBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
}

static void parseEntityDecl(EntityDecl* eDecl){
#ifdef DEBUG
	memcpy(&(eDecl->token), &(p->currToken), sizeof(Token));
#endif

	nextToken();	
	if(!match(TOKEN_IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	eDecl->name = (Identifier*)parseIdentifier();

	nextToken();
	if(!match(TOKEN_LBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	if(!peek(TOKEN_RBRACE)){
		eDecl->ports = parsePortDecl();	
	} else {
		nextToken();
	}

	if(!match(TOKEN_RBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
		printf("TokenDump: %d %s\r\n", p->currToken.type ,p->currToken.literal);		
	}
}

static UseStatement* parseUseStatement(){
	UseStatement* stmt = malloc(sizeof(UseStatement));

#ifdef DEBUG	
	memcpy(&(stmt->token), &(p->currToken), sizeof(Token));
#endif

	if(!match(TOKEN_USE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
		
	nextToken();	
	if(!match(TOKEN_IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	
	int size = strlen(p->currToken.literal) + 1;
	stmt->value = malloc(sizeof(char) * size);
	memcpy(stmt->value, p->currToken.literal, size);

	return stmt;
}

static DesignUnit* parseDesignUnit(){

	DesignUnit* unit = calloc(1, sizeof(DesignUnit));

	switch(p->currToken.type){
		case TOKEN_ENT: { 
			unit->type = ENTITY;
			parseEntityDecl(&(unit->as.entity));
			break;
		}

		case TOKEN_ARCH: {
			unit->type = ARCHITECTURE;
			parseArchitectureDecl(&(unit->as.architecture));
			break;
		}

		default:
			free(unit);
			unit = NULL;
			break;
	}

	return unit;
}

Program* ParseProgram(){
	Program* prog = calloc(1, sizeof(Program));

	// first get the use statements
	while(p->currToken.type == TOKEN_USE && p->currToken.type != TOKEN_EOP){
		UseStatement* stmt = parseUseStatement();
		if(stmt != NULL){
			if(prog->useStatements == NULL){
				prog->useStatements = initBlockArray(sizeof(UseStatement));
			}
			writeBlockArray(prog->useStatements, (char*)stmt);
			free(stmt);
		}
		nextToken();
	}

	// next parse any design units
	while(p->currToken.type != TOKEN_EOP){
		DesignUnit* unit = parseDesignUnit();
		if(unit != NULL){
			if(prog->units == NULL){
				prog->units = initBlockArray(sizeof(DesignUnit));	
			}
			writeBlockArray(prog->units, (char*)unit);
			free(unit);
		}
		nextToken();
	}
	
	return prog;
}

// I know this isn't portable, but I just love it so much 
#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})

static void freeExpression(void* expr){
	 ExpressionType type = ((Expression*)expr)->type;

	switch(type) {

		case CHAR_EXPR: {
			CharExpr* chexp = (CharExpr*)expr;
			free(chexp->literal);
			free(chexp);
			break;
		}

		default:
		break;
	}
}

void FreeProgram(Program* prog){
	
	// setup block
	OperationBlock* opBlk = initOperationBlock();
	opBlk->doProgOp 			= lambda (void, (void* prog) 	{ Program* pg = (Program*)prog; pg->useStatements=NULL; pg->units=NULL; free(pg); });
	opBlk->doUseStatementOp = lambda (void, (void* stmt) 	{ UseStatement* st = (UseStatement*)stmt; if(st->value) free(st->value); });
	opBlk->doBlockArrayOp 	= lambda (void, (void* arr) 	{ freeBlockArray((Dba*)arr); });
	opBlk->doIdentifierOp	= lambda (void, (void* ident) { Identifier* id = (Identifier*)ident; if(id->value) free(id->value); free(id); });
	opBlk->doPortModeOp 		= lambda (void, (void* pmode) { PortMode* pm = (PortMode*)pmode; if(pm->value) free(pm->value); free(pm); });
	opBlk->doDataTypeOp 		= lambda (void, (void* dtype) { DataType* dt = (DataType*)dtype; if(dt->value) free(dt->value); free(dt); });
	opBlk->doExpressionOp	= freeExpression;
	
	WalkTree(prog, opBlk);

	if(p->currToken.literal) free(p->currToken.literal);
	if(p->peekToken.literal) free(p->peekToken.literal);

	free(opBlk);
}
