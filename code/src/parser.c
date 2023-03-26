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

	statement		->			sigAssign
									| varAssign

	sigAssign		->			(LABEL ":")? IDENTIFIER "<=" expression ";" ; 
	
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

// I know this isn't portable, but I just love it so much 
#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})

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

static bool validDataType(){
	bool valid = false; 
	
	valid = 	match(STL) 		||	match(STLV) 	||
				match(INTEGER) || match(STRING) 	||
				match(BIT) 		|| match(BITV)		||
				match(SIGNED)	|| match(UNSIGNED);

	return valid;
}

static void* parseExpression(void){
	return NULL;
}

static DataType* parseDataType(char* val){
	DataType* dt = calloc(1, sizeof(DataType));

	int size = strlen(val) + 1;
	dt->value = calloc(size, sizeof(char));
	memcpy(dt->value, val, size);

	return dt;
}

static PortMode* parsePortMode(char* val){
	PortMode* pm = calloc(1, sizeof(PortMode));

	int size = strlen(val) +1;
	pm->value = calloc(size, sizeof(char));
	memcpy(pm->value, val, size);

	return pm;
}

static Identifier* parseIdentifier(char* val){
	Identifier* ident = calloc(1, sizeof(Identifier));

	int size = strlen(val) + 1;
	ident->value = calloc(size, sizeof(char));
	memcpy(ident->value, val, size);
	
	return ident;
}

static Dba* parseArchBodyStatements(){
	return NULL;
}

static Dba* parseArchBodyDeclarations(){
	Dba* decls = initBlockArray(sizeof(SignalDecl));

	nextToken();	
	
	while(match(SIG)){
		SignalDecl* decl = calloc(1, sizeof(SignalDecl));

		nextToken();
		if(!match(IDENTIFIER)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		decl->name = parseIdentifier(p->currToken.literal);
		
		nextToken();
		if(!validDataType()){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		decl->dtype = parseDataType(p->currToken.literal);
		
		if(p->peekToken.type == VASSIGN){
			decl->expression = parseExpression();	
		} else {
			nextToken();
		}

		if(!match(SCOLON)){
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

	while(!match(RBRACE) && !match(EOP)){
		PortDecl* port = malloc(sizeof(PortDecl));		

		if(!match(IDENTIFIER)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		port->name = parseIdentifier(p->currToken.literal);
		
		nextToken();
		if(!match(INPUT) && !match(OUTPUT) && !match(INOUT)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		port->pmode = parsePortMode(p->currToken.literal);
		
		nextToken();
		if(!validDataType()){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
		}	
		port->dtype = parseDataType(p->currToken.literal);
		
		nextToken();
		if(!match(SCOLON)){
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
	if(!match(IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	aDecl->archName = parseIdentifier(p->currToken.literal);
	
	nextToken();
	if(!match(LPAREN)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	nextToken();	
	if(!match(IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	aDecl->entName = parseIdentifier(p->currToken.literal);
	
	nextToken();
	if(!match(RPAREN)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	nextToken();
	if(!match(LBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	if(p->peekToken.type != RBRACE){
		aDecl->declarations = parseArchBodyDeclarations();	
		aDecl->statements = parseArchBodyStatements();	
	} else {
		nextToken();
	}

	if(!match(RBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
}

static void parseEntityDecl(EntityDecl* eDecl){
#ifdef DEBUG
	memcpy(&(eDecl->token), &(p->currToken), sizeof(Token));
#endif

	nextToken();	
	if(!match(IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	eDecl->name = parseIdentifier(p->currToken.literal);

	nextToken();
	if(!match(LBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	if(p->peekToken.type != RBRACE){
		eDecl->ports = parsePortDecl();	
	} else {
		nextToken();
	}

	if(!match(RBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
		printf("TokenDump: %d %s\r\n", p->currToken.type ,p->currToken.literal);		
	}
}

static UseStatement* parseUseStatement(){
	UseStatement* stmt = malloc(sizeof(UseStatement));

#ifdef DEBUG	
	memcpy(&(stmt->token), &(p->currToken), sizeof(Token));
#endif

	if(!match(USE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
		
	nextToken();	
	if(!match(IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	
	int size = strlen(p->currToken.literal) + 1;
	stmt->value = malloc(sizeof(char) * size);
	memcpy(stmt->value, p->currToken.literal, size);

	return stmt;
}

static DesignUnit* parseDesignUnit(){

	switch(p->currToken.type){
		case ENT: { 
			DesignUnit* unit = calloc(1, sizeof(DesignUnit));
			unit->type = ENTITY;
			parseEntityDecl(&(unit->decl.entity));
			return unit;
		}

		case ARCH: {
			DesignUnit* unit = calloc(1, sizeof(DesignUnit));
			unit->type = ARCHITECTURE;
			parseArchitectureDecl(&(unit->decl.architecture));
			return unit;
		}

		default:
			break;
	}

	return NULL;
}

Program* ParseProgram(){
	Program* prog = calloc(1, sizeof(Program));

	// first get the use statements
	while(p->currToken.type == USE && p->currToken.type != EOP){
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
	while(p->currToken.type != EOP){
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

void FreeProgram(Program* prog){
	
	// setup block
	OperationBlock* opBlk = initOperationBlock();
	opBlk->doProgOp 			= lambda (void, (void* prog) 	{ Program* pg = (Program*)prog; pg->useStatements=NULL; pg->units=NULL; free(pg); });
	opBlk->doUseStatementOp = lambda (void, (void* stmt) 	{ UseStatement* st = (UseStatement*)stmt; if(st->value) free(st->value); });
	opBlk->doBlockArrayOp 	= lambda (void, (void* arr) 	{ freeBlockArray((Dba*)arr); });
	opBlk->doIdentifierOp	= lambda (void, (void* ident) { Identifier* id = (Identifier*)ident; if(id->value) free(id->value); free(id); });
	opBlk->doPortModeOp 		= lambda (void, (void* pmode) { PortMode* pm = (PortMode*)pmode; if(pm->value) free(pm->value); free(pm); });
	opBlk->doDataTypeOp 		= lambda (void, (void* dtype) { DataType* dt = (DataType*)dtype; if(dt->value) free(dt->value); free(dt); });
	
	WalkTree(prog, opBlk);

	if(p->currToken.literal) free(p->currToken.literal);
	if(p->peekToken.literal) free(p->peekToken.literal);

	free(opBlk);
}
