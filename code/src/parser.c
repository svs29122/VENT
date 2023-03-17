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

static Identifier* parseIdentifier(char* val){
	Identifier* ident = malloc(sizeof(Identifier));
	
	int size = strlen(val);
	ident->value = malloc(sizeof(size));
	memcpy(ident->value, val, size);

	return ident;
}

static void parseArchitectureDecl(ArchitectureDecl* decl){
	
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

	if(p->peekToken.type != RBRACE){
		nextToken();
	}
	eDecl->ports = parsePortDecl();	

   nextToken();
	if(!match(RBRACE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
}

static UseStatement* parseUseStatement(){
	UseStatement* stmt = malloc(sizeof(UseStatement));

#ifdef DEBUG	
	memcpy(&(stmt->token), &token, sizeof(Token));
#endif

	if(!match(USE)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
		
	nextToken();	
	if(!match(IDENTIFIER)){
		printf("Error: %s:%d\r\n", __func__, __LINE__);		
	}
	
	stmt->value = malloc(strlen(p->currToken.literal));
	memcpy(stmt->value, p->currToken.literal, strlen(p->currToken.literal));
	
	return stmt;
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
		UseStatement* stmt = parseUseStatement();
		if(stmt != NULL){
			printf("Got Use Statment\r\n");
			if(prog->useStatements == NULL){
				prog->useStatements = malloc(sizeof(Dba));
				printf("Init Use Statment Block Array\r\n");
				initBlockArray(prog->useStatements, sizeof(UseStatement));
			}
			printf("Write Use Statement Block Array\r\n");
			writeBlockArray(prog->useStatements, (char*)stmt);
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
	
	//PrintProgram(prog);

	return prog;
}

void FreeProgram(Program *prog){
	if(prog){
		if(prog->useStatements){
			Dba* arr = prog->useStatements;
			for(int i=0; i < arr->count; i++){
				UseStatement* stmt = (UseStatement*)(arr->block + (i * arr->blockSize));
				if(stmt->value){
					free(stmt->value);
				}
			}
			freeBlockArray(arr);
			free(prog->useStatements);
		}
		if(prog->units){
			switch(prog->units->type){
				case ENTITY:
					if(prog->units->decl.entity.name){
						if(prog->units->decl.entity.name->value){
							free(prog->units->decl.entity.name->value);
						}	
						free(prog->units->decl.entity.name);
					}
					if(prog->units->decl.entity.ports){
						if(prog->units->decl.entity.ports->names){
							free(prog->units->decl.entity.ports->names->value);
						}
						if(prog->units->decl.entity.ports->pmode.value){
							free(prog->units->decl.entity.ports->pmode.value);
						}
						free(prog->units->decl.entity.ports);
					}
				case ARCHITECTURE:
					break;
				default:
					break;
			}
			free(prog->units);
		}
	}// endif prog
}

char shift(int c){
	printf("\e[0;34m|");
	for(int i=0; i<c-1; i++){
		printf("-");	
	}
	printf("\e[0m");
	return ' ';
}

void PrintProgram(Program* prog){
	if(prog){
		printf("\e[1;32m""Program\r\n");
	}
	if(prog->useStatements){	
		Dba* arr = prog->useStatements;
		for(int i=0; i < arr->count; i++){
			UseStatement* stmt = (UseStatement*)(arr->block + (i * arr->blockSize));
			if(stmt->value){
				printf("\e[1;36m""%cUseStatement: %s\r\n",shift(1), stmt->value);
			}
		}
	}
	if(prog->units){
		DesignUnit* unit = prog->units;
		
		switch(unit->type){
			case ENTITY:
				printf("\e[1;32m""%cDesignUnit\r\n", shift(1));
				EntityDecl* eDecl = &(unit->decl.entity);
				printf("\e[0;32m""%cEntityDecl\r\n", shift(2));
				if(eDecl->name){
						if(eDecl->name->value){
							printf("\e[0;35m""%cIdentifier: \'%s\'\r\n", shift(3), eDecl->name->value);
						}
				}
				break;
			
			case ARCHITECTURE: 
				break;
			
			default: 
				break;
			
		}
	}
	printf("\e[0m");
}
