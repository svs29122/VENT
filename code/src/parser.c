/*
	parser.c

	This file contains all code used to parse the list of tokens 
	produced by the lexer. The end result is an abstract syntax
	tree representing a VENT program/file.
	--
*/

#include <stdio.h>

#include "token.h"
#include "lexer.h"
#include "ast.h"


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

struct parser {
	struct lexer* l;
	Token curToken;
	Token peekToken;
};

struct parser* NewParser(struct lexer *lex){
	struct parser* newParser = (struct parser*)malloc(sizeof(struct parser));

	newParser->l = lex;
	newParser->curToken = lex->NextToken();
	newParser->peekToken = lex->NextToken();	

	return newParser;
}

Program* ParseProgram(struct parser *par){
	Program* prog = (Program*)malloc(sizeof(Program));
	prog->statements = NULL;

	while(par->nextToken() != EOP){
		// statement = parseStatement();
		if(statement != NULL){
			//prog->statements = add(statement);
		}
		par->NexToken();
	}
	
	return prog;
}
