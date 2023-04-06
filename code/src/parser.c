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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "token.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"

bool printTokenFlag = false;

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
static struct Expression* parseCharlit();

struct ParseRule rules[] = {
	[TOKEN_AND] = {NULL, parseBinary, LOGICAL_PREC},
	[TOKEN_IDENTIFIER] = {parseIdentifier, NULL, LOWEST_PREC},
	[TOKEN_CHARLIT] = {parseCharlit, NULL, LOWEST_PREC},
};

static struct ParseRule* getRule(enum TOKEN_TYPE type){
	return &rules[type];
}

struct parser {
	struct Token currToken;
	struct Token peekToken;
	bool printTokenFlag;
} static parser;

static struct parser *p = &parser;

void InitParser(){
	
	memset(p, 0, sizeof(struct parser));

	p->currToken = NextToken();
	p->peekToken = NextToken();	
}

void SetPrintTokenFlag(){
	printTokenFlag = true;
}

static struct Token nextToken(){	
	
	if(printTokenFlag) PrintToken(p->currToken);

	free(p->currToken.literal);

	p->currToken = p->peekToken;
	p->peekToken = NextToken();
}

static bool match(enum TOKEN_TYPE type){
	return p->currToken.type == type;
}

static bool peek(enum TOKEN_TYPE type){
	return p->peekToken.type == type;
}

static void consume(enum TOKEN_TYPE type, const char* msg){
	if(!match(type)){
		error(p->currToken.lineNumber, p->currToken.literal, msg);
		
		// if the next token is the one we expect move to it
		if(peek(type)) {
			nextToken();
		} else {
			while(!peek(TOKEN_RBRACE) && !peek(TOKEN_EOP) && !match(TOKEN_EOP)){
				nextToken();
			}
		}
	}
}

static void consumeNext(enum TOKEN_TYPE type, const char* msg){
	nextToken();
	consume(type, msg);
}

static bool validDataType(){
	bool valid = false; 
	
	valid = 	match(TOKEN_STL) 		||	match(TOKEN_STLV) 	||
				match(TOKEN_INTEGER) || match(TOKEN_STRING) 	||
				match(TOKEN_BIT) 		|| match(TOKEN_BITV)		||
				match(TOKEN_SIGNED)	|| match(TOKEN_UNSIGNED);

	return valid;
}

static struct Expression* parseExpression(enum Precedence precedence){
	ParsePrefixFn prefixRule = getRule(p->currToken.type)->prefix;

	if(prefixRule == NULL){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect l-value expression");
		return NULL;
	}	
	struct Expression* leftExp = prefixRule();

	nextToken();	
	while(!peek(TOKEN_SCOLON) && precedence < getRule(p->currToken.type)->precedence){
		ParseInfixFn infixRule = getRule(p->currToken.type)->infix;

		if(infixRule == NULL){
			return leftExp;
		}

		leftExp = infixRule(leftExp);
	}

	if(peek(TOKEN_SCOLON)){
		nextToken();
	}

	return leftExp;
}

static struct Expression* parseIdentifier(){
	struct Identifier* ident = calloc(1, sizeof(struct Identifier));
#ifdef DEBUG
	memcpy(&(ident->self.token), &(p->currToken), sizeof(struct Token));
#endif

	ident->self.type = NAME_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	ident->value = calloc(size, sizeof(char));
	memcpy(ident->value, p->currToken.literal, size);
	
	return &(ident->self);
}

static struct Expression* parseCharlit(){
	struct CharExpr* chexp = calloc(1, sizeof(struct CharExpr));
#ifdef DEBUG
	memcpy(&(chexp->self.token), &(p->currToken), sizeof(struct Token));
#endif

	chexp->self.type = CHAR_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	chexp->literal = calloc(size, sizeof(char));
	memcpy(chexp->literal, p->currToken.literal, size);
	
	return &(chexp->self);
}

static struct Expression* parseBinary(struct Expression* expr){
	struct BinaryExpr* biexp = calloc(1, sizeof(struct BinaryExpr));
#ifdef DEBUG
	memcpy(&(biexp->self.token), &(p->currToken), sizeof(struct Token));
#endif

	biexp->self.type = BINARY_EXPR;
	biexp->left = expr;

	int size = strlen(p->currToken.literal) + 1;
	biexp->op = calloc(size, sizeof(char));
	memcpy(biexp->op, p->currToken.literal, size);

	enum Precedence precedence = getRule(p->currToken.type)->precedence;
	nextToken();
	
	biexp->right = parseExpression(precedence);

	return &(biexp->self);
}

static struct DataType* parseDataType(char* val){
	struct DataType* dt = calloc(1, sizeof(struct DataType));
#ifdef DEBUG
	memcpy(&(dt->token), &(p->currToken), sizeof(struct Token));
#endif

	int size = strlen(val) + 1;
	dt->value = calloc(size, sizeof(char));
	memcpy(dt->value, val, size);

	return dt;
}

static struct PortMode* parsePortMode(char* val){
	struct PortMode* pm = calloc(1, sizeof(struct PortMode));
#ifdef DEBUG
	memcpy(&(pm->token), &(p->currToken), sizeof(struct Token));
#endif

	int size = strlen(val) +1;
	pm->value = calloc(size, sizeof(char));
	memcpy(pm->value, val, size);

	return pm;
}

static Dba* parseArchBodyStatements(){
	Dba* stmts = InitBlockArray(sizeof(struct SignalAssign));
	
	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		struct SignalAssign* stmt = calloc(1, sizeof(struct SignalAssign));

		consume(TOKEN_IDENTIFIER, "expect identifer at start of statement");
		stmt->target = (struct Identifier*)parseIdentifier();

		consumeNext(TOKEN_SASSIGN, "Expect <= after identifier");
		
		nextToken();
		stmt->expression = parseExpression(LOWEST_PREC);

		consume(TOKEN_SCOLON, "Expect semicolon at end of signal assignment");	
		WriteBlockArray(stmts, (char*)stmt);
		free(stmt);
	
		nextToken();	
	}

	return stmts;
}

static Dba* parseArchBodyDeclarations(){
	Dba* decls = InitBlockArray(sizeof(struct SignalDecl));

	while(match(TOKEN_SIG)){
		struct SignalDecl* decl = calloc(1, sizeof(struct SignalDecl));

		consumeNext(TOKEN_IDENTIFIER, "Expect identifier after sig keyword in signal declaration");
		decl->name = (struct Identifier*)parseIdentifier();
		
		nextToken();
		if(!validDataType()){
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid data type after signal identifier");
		}	
		decl->dtype = parseDataType(p->currToken.literal);
		
		nextToken();
		if(match(TOKEN_VASSIGN)){
			nextToken();
			decl->expression = parseExpression(LOWEST_PREC);	
		}

		consume(TOKEN_SCOLON, "Expect semicolon at end of signal declaration");
		WriteBlockArray(decls, (char*)decl);
		free(decl);
	
		nextToken();	
	}
	
	return decls;
}

static Dba* parsePortDecl(){
	Dba* ports = InitBlockArray(sizeof(struct PortDecl)); 	

	nextToken();

	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		struct PortDecl* port = malloc(sizeof(struct PortDecl));		

		consume(TOKEN_IDENTIFIER, "Expect identifier at start of port declaration");
		port->name = (struct Identifier*)parseIdentifier();
		
		nextToken();
		if(!match(TOKEN_INPUT) && !match(TOKEN_OUTPUT) && !match(TOKEN_INOUT)){
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid port mode");
		}	
		port->pmode = parsePortMode(p->currToken.literal);
		
		nextToken();
		if(!validDataType()){
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid data type");
		}	
		port->dtype = parseDataType(p->currToken.literal);
		
		consumeNext(TOKEN_SCOLON, "Expect ; at end of port declaration");
		WriteBlockArray(ports, (char*)port);
		free(port);
		
		nextToken();
	}

	return ports;
}

static void parseArchitectureDecl(struct ArchitectureDecl* aDecl){
#ifdef DEBUG
	memcpy(&(aDecl->token), &(p->currToken), sizeof(struct Token));
#endif
	
	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after keywordk arch");
	aDecl->archName = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_LPAREN, "Expect ( after architecture identifier");

	consumeNext(TOKEN_IDENTIFIER, "Expect entity identifier after ( in architecture declaration");
	aDecl->entName = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_RPAREN, "Expect ) after entity identifier in architecture declaration");
	consumeNext(TOKEN_LBRACE, "Expect { after architecture identifier declaration and before architecture body");  

	nextToken();
	if(!match(TOKEN_RBRACE)){
		aDecl->declarations = parseArchBodyDeclarations();	
		aDecl->statements = parseArchBodyStatements();	
	}

	consume(TOKEN_RBRACE, "Expect } after architecture body");
}

static void parseEntityDecl(struct EntityDecl* eDecl){
#ifdef DEBUG
	memcpy(&(eDecl->token), &(p->currToken), sizeof(struct Token));
#endif
	
	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after ent keyword");
	eDecl->name = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_LBRACE, "Expect { after entity identifier");

	if(!peek(TOKEN_RBRACE)){
		eDecl->ports = parsePortDecl();	
	} else {
		nextToken();
	}

	consume(TOKEN_RBRACE, "Expect } at the end of entity declaration");
}

static struct UseStatement* parseUseStatement(){
	struct UseStatement* stmt = malloc(sizeof(struct UseStatement));

#ifdef DEBUG	
	memcpy(&(stmt->token), &(p->currToken), sizeof(struct Token));
#endif

	consumeNext(TOKEN_IDENTIFIER, "Expect use path after use keyword");
	
	int size = strlen(p->currToken.literal) + 1;
	stmt->value = malloc(sizeof(char) * size);
	memcpy(stmt->value, p->currToken.literal, size);

	return stmt;
}

static struct DesignUnit* parseDesignUnit(){

	struct DesignUnit* unit = calloc(1, sizeof(struct DesignUnit));

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
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid design unit type");
			free(unit);
			unit = NULL;
			break;
	}

	return unit;
}

struct Program* ParseProgram(){
	struct Program* prog = calloc(1, sizeof(struct Program));

	// first get the use statements
	while(p->currToken.type == TOKEN_USE && p->currToken.type != TOKEN_EOP){
		struct UseStatement* stmt = parseUseStatement();
		if(stmt != NULL){
			if(prog->useStatements == NULL){
				prog->useStatements = InitBlockArray(sizeof(struct UseStatement));
			}
			WriteBlockArray(prog->useStatements, (char*)stmt);
			free(stmt);
		}
		nextToken();
	}

	// next parse any design units
	while(p->currToken.type != TOKEN_EOP){
		struct DesignUnit* unit = parseDesignUnit();
		if(unit != NULL){
			if(prog->units == NULL){
				prog->units = InitBlockArray(sizeof(struct DesignUnit));	
			}
			WriteBlockArray(prog->units, (char*)unit);
			free(unit);
		} else {
			break;
		}
		nextToken();
	}
	
	return prog;
}

static void freeExpression(void* expr){
   enum ExpressionType type = ((struct Expression*)expr)->type;

   switch(type) {

      case CHAR_EXPR: {
         struct CharExpr* chexp = (struct CharExpr*)expr;
         free(chexp->literal);
			free(chexp);
         break;
      }

      case BINARY_EXPR:{
         struct BinaryExpr* bexp = (struct BinaryExpr*) expr;
         freeExpression((void*)bexp->left);
         free(bexp->op);
         freeExpression((void*)bexp->right);
			free(bexp);
         break;
      }

      case NAME_EXPR: {
         //NameExpr* nexp = (NameExpr*) expr;
         //free(nexp->name->value);
         struct Identifier* ident = (struct Identifier*)expr;
         free(ident->value);
         free(ident);
         break;
      }

      default:
         break;
   }
}

// I know this isn't portable, but I just love it so much 
#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})

void FreeProgram(struct Program* prog){
	
	// setup block
	struct OperationBlock* opBlk = InitOperationBlock();
	opBlk->doProgOp 			= lambda (void, (void* prog) 	{ struct Program* pg = (struct Program*)prog; pg->useStatements=NULL; pg->units=NULL; free(pg); });
	opBlk->doUseStatementOp = lambda (void, (void* stmt) 	{ struct UseStatement* st = (struct UseStatement*)stmt; if(st->value) free(st->value); });
	opBlk->doBlockArrayOp 	= lambda (void, (void* arr) 	{ FreeBlockArray((Dba*)arr); });
	opBlk->doIdentifierOp	= lambda (void, (void* ident) { struct Identifier* id = (struct Identifier*)ident; if(id->value) free(id->value); free(id); });
	opBlk->doPortModeOp 		= lambda (void, (void* pmode) { struct PortMode* pm = (struct PortMode*)pmode; if(pm->value) free(pm->value); free(pm); });
	opBlk->doDataTypeOp 		= lambda (void, (void* dtype) { struct DataType* dt = (struct DataType*)dtype; if(dt->value) free(dt->value); free(dt); });
	opBlk->doExpressionOp	= freeExpression;
	
	WalkTree(prog, opBlk);

	if(p->currToken.literal) free(p->currToken.literal);
	if(p->peekToken.literal) free(p->peekToken.literal);

	free(opBlk);
}
