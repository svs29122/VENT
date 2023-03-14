/*
	parser.c

	This file contains all code used to parse the list of tokens 
	produced by the lexer. The end result is an abstract syntax
	tree representing a VENT program/file.
	--
*/

////////////////////////////////////////////////////////////////
/*
	The VENT Grammar
	-----------------

	program 			->			useStmt* designUnit* EOF;

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

struct parser {
	Token currToken;
	Token peekToken;
} static parser;

static struct parser *p = &parser;

void InitParser(){

	p->currToken = NextToken();
	p->peekToken = NextToken();	
}

static Token nextToken(){
	p->currToken = p->peekToken;
	p->peekToken = NextToken();
}

static bool match( enum TOKEN_TYPE type){
	return p->currToken.type == type;
}

static UseStatement* parseUseStatement(Token token){
	UseStatement* stmt = malloc(sizeof(UseStatement));

#ifdef DEBUG	
	memcpy(&(stmt->token), &token, sizeof(Token));
#endif
	memcpy(stmt->value, token.literal, strlen(token.literal));
	
	return stmt;
}

static Identifier* parseIdentifier(char* val){
	Identifier* ident = malloc(sizeof(Identifier));
	
	int size = strlen(val);
	ident->value = malloc(sizeof(size));
	memcpy(ident->value, val, size);

	return ident;
}

static PortDecl* parsePortDecl(){
	PortDecl* pdecl = NULL;

	return pdecl;
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

	nextToken();
	eDecl->ports = parsePortDecl();	
	
	if(eDecl->ports != NULL) nextToken();

	if(!match(RBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
}

static void parseArchitectureDecl(ArchitectureDecl* decl){
	
}

static DesignUnit* parseDesignUnit(){

	switch(p->currToken.type){
		case ENT: { 
			DesignUnit* unit = malloc(sizeof(DesignUnit));
			unit->type = ENTITY;
			parseEntityDecl(&(unit->decl.entity));
			return unit;
		}

		case ARCH: {
			DesignUnit* unit = malloc(sizeof(DesignUnit));
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
	Program* prog = malloc(sizeof(Program));

	// first get the use statements
	while(p->currToken.type == USE && p->currToken.type != EOP){
		UseStatement* stmt = parseUseStatement(p->currToken);
		if(stmt != NULL){
			prog->statements = malloc(sizeof(UseStatement));
			memcpy(prog->statements, stmt, sizeof(UseStatement));
		}
		nextToken();
	}

	// next parse any design units
	while(p->currToken.type != EOP){
		DesignUnit* unit = parseDesignUnit();
		if(unit != NULL){
			prog->units = malloc(sizeof(DesignUnit));
			memcpy(prog->units, unit, sizeof(DesignUnit));
		}
		nextToken();
	}
	
	return prog;
}

void PrintProgram(Program* prog){
	if(prog){
		printf("<Program>\r\n");
	}
	if(prog->statements){
		printf("\t<UseStatement>: %s\r\n", prog->statements->value);
	}
	if(prog->units){
		DesignUnit* unit = prog->units;
		
		switch(unit->type){
			case ENTITY:
				printf("\t<DesignUnit>\r\n");
				EntityDecl* eDecl = &(unit->decl.entity);
				printf("\t\t<EntityDecl>\r\n");
				if(eDecl->name){
						if(eDecl->name->value){
							printf("\t\t\t<Identifier>: %s\r\n", eDecl->name->value);
						}
				}
				break;
			
			case ARCHITECTURE: 
				break;
			
			default: 
				break;
			
		}
	}
}
