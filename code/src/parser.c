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

static void parseArchitectureDecl(ArchitectureDecl* decl){
	
}

static Dba* parsePortDecl(){
	Dba* ports = initBlockArray(sizeof(PortDecl)); 	

	nextToken();

	while(!match(RBRACE) && !match(EOP)){
		PortDecl* port = malloc(sizeof(PortDecl));		

		if(!match(IDENTIFIER)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
			printf("TokenDump: %d %s\r\n", p->currToken.type ,p->currToken.literal);		
		}	
		port->name = parseIdentifier(p->currToken.literal);
		
		nextToken();
		if(!match(INPUT) && !match(OUTPUT) && !match(INOUT)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
			printf("TokenDump: %d %s\r\n", p->currToken.type ,p->currToken.literal);		
		}	
		port->pmode = parsePortMode(p->currToken.literal);
		
		nextToken();
		if(!validDataType()){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
			printf("TokenDump: %d %s\r\n", p->currToken.type ,p->currToken.literal);		
		}	
		port->dtype = parseDataType(p->currToken.literal);
		
		nextToken();
		if(!match(SCOLON)){
			printf("Error: %s:%d\r\n", __func__, __LINE__);		
			printf("TokenDump: %d %s\r\n", p->currToken.type ,p->currToken.literal);		
		}	
		writeBlockArray(ports, (char*)port);
		free(port);
		
		nextToken();
	}

	return ports;
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
	
	//PrintProgram(prog);

	return prog;
}

void FreeProgram(Program *prog){
	if(prog){
		if(prog->useStatements){
			Dba* arr = prog->useStatements;
			for(int i=0; i < arr->count; i++){
				UseStatement* stmt = (UseStatement*)(arr->block + (i * arr->blockSize));
				if(stmt){
					if(stmt->value){
						free(stmt->value);
					}
				}
			}
			freeBlockArray(arr);
			prog->useStatements = NULL;
		}
		if(prog->units){
			Dba* arr = prog->units;
			for(int i=0; i < arr->count; i++){
				DesignUnit* unit = (DesignUnit*)(arr->block + (i * arr->blockSize));
				switch(unit->type){
					case ENTITY:
						if(unit->decl.entity.name){
							if(unit->decl.entity.name->value){
								free(unit->decl.entity.name->value);
								unit->decl.entity.name->value = NULL;
							}	
							free(unit->decl.entity.name);
							unit->decl.entity.name = NULL;
						}
						if(unit->decl.entity.ports){
							Dba* ports = unit->decl.entity.ports;
							for(int i=0; i < ports->count; i++){
								PortDecl* port = (PortDecl*)(ports->block + (i * ports->blockSize));
								if(port->name){
									if(port->name->value){
										free(port->name->value);
									}
									free(port->name);
								}
								if(port->pmode){
									if(port->pmode->value){
										free(port->pmode->value);
									}
									free(port->pmode);
								}
								if(port->dtype){
									if(port->dtype->value){
										free(port->dtype->value);
									}
									free(port->dtype);
								}
							}
							freeBlockArray(ports);
							unit->decl.entity.ports = NULL;
						}
					case ARCHITECTURE:
						break;
					default:
						break;
				}
			}
			freeBlockArray(arr);
			prog->units = NULL;
		}
		free(prog);
		prog = NULL;
	}// endif prog
	
	if(p->currToken.literal) free(p->currToken.literal);
	if(p->peekToken.literal) free(p->peekToken.literal);
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
		Dba* arr = prog->units;
		for(int i=0; i < arr->count; i++){
			DesignUnit* unit = (DesignUnit*)(arr->block + (i * arr->blockSize));
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
					if(eDecl->ports){
						Dba* ports = eDecl->ports;
						for(int i=0; i < ports->count; i++){
							printf("\e[0;32m""%cPortDecl\r\n", shift(3));
							PortDecl* port = (PortDecl*)(ports->block + (i * ports->blockSize));
							if(port->name->value){
								printf("\e[0;35m""%cIdentifier: \'%s\'\r\n", shift(4), port->name->value);
							}
							if(port->pmode->value){
								printf("\e[0;35m""%cPortMode: \'%s\'\r\n", shift(4), port->pmode->value);
							}
							if(port->dtype->value){
								printf("\e[0;35m""%cDataType: \'%s\'\r\n", shift(4), port->dtype->value);
							}
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
	printf("\e[0m");
}
