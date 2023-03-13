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
#include "ast.h"

struct parser {
	struct lexer* l;
	Token currToken;
	Token peekToken;
};

struct parser* NewParser(struct lexer *lex){
	struct parser* newParser = (struct parser*)malloc(sizeof(struct parser));

	newParser->l = lex;
	newParser->currToken = NextToken(newParser->l);
	newParser->peekToken = NextToken(newParser->l);	

	return newParser;
}

static Token nextToken(struct parser* p){
	p->currToken = p->peekToken;
	p->peekToken = NextToken(p->l);
}

static bool match(Token tok, enum TOKEN_TYPE type){
	return tok.type == type;
}

static UseStatement* parseUseStatement(Token token){
	UseStatement* stmt = malloc(sizeof(UseStatement));

#ifdef DEBUG	
	memcpy(&(stmt->token), &token, sizeof(Token));
#endif
	memcpy(stmt->value, token.literal, strlen(token.literal));
	
	return stmt;
}

static PortDecl* parsePortDecl(struct parser* p){
	PortDecl* pdecl = NULL;

	return pdecl;
}

static void parseEntityDecl(EntityDecl* decl, struct parser* p){
#ifdef DEBUG
	memcpy(&(decl->token), &(p->currToken), sizeof(Token));
#endif

	nextToken(p);	
	if(!match(p->currToken, IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	memcpy(decl->name->value, p->currToken.literal, strlen(p->currToken.literal));

	nextToken(p);
	if(!match(p->currToken, LBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}

	nextToken(p);
	decl->ports = parsePortDecl(p);	
	
	nextToken(p);
	if(!match(p->currToken, RBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
}

static void parseArchitectureDecl(ArchitectureDecl* decl, struct parser* p){
	
}

static DesignUnit* parseDesignUnit(struct parser* p){

	switch(p->currToken.type){
		case ENT: { 
			DesignUnit* unit = malloc(sizeof(DesignUnit));
			unit->type = ENTITY;
			parseEntityDecl(&(unit->decl.entity), p);
			return unit;
		}

		case ARCH: {
			DesignUnit* unit = malloc(sizeof(DesignUnit));
			unit->type = ARCHITECTURE;
			parseArchitectureDecl(&(unit->decl.architecture), p);
			return unit;
		}

		default:
			break;
	}

	return NULL;
}

Program* ParseProgram(struct parser *p){
	Program* prog = malloc(sizeof(Program));

	// first get the use statements
	while(p->currToken.type == USE && p->currToken.type != EOP){
		UseStatement* stmt = parseUseStatement(p->currToken);
		if(stmt != NULL){
			//TODO: using first member temporarily
			memcpy(&(prog->statements[0]), &stmt, sizeof(UseStatement));
		}
		nextToken(p);
	}

	// next parse any design units
	while(p->currToken.type != EOP){
		DesignUnit* unit = parseDesignUnit(p);
		if(unit != NULL){
			//TODO: using first member temporarily
			memcpy(&(prog->units[0]), &unit, sizeof(DesignUnit));
		}
		nextToken(p);
	}
	
	return prog;
}
