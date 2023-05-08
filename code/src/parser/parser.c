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
									| numericLiteral
									| basedLiteral
									| physicalLiteral
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
	
	numericLiteral	->			INTEGER | REAL

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
#include <stdio.h>

#include <token.h>
#include <lexer.h>
#include <ast.h>

static bool printTokenFlag = false;

struct parser {
	struct Token currToken;
	struct Token peekToken;
} static parser;

static struct parser *p = &parser;

//private includes
#include "error.h"
#include "expression.h"
#include "utils.h"
#include "free.h"

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

static struct Expression* parseCharLiteral(){
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

static struct Expression* parseNumericLiteral(){
	struct NumExpr* nexp = calloc(1, sizeof(struct NumExpr));
#ifdef DEBUG
	memcpy(&(nexp->self.token), &(p->currToken), sizeof(struct Token));
#endif

	nexp->self.type = NUM_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	nexp->literal = calloc(size, sizeof(char));
	memcpy(nexp->literal, p->currToken.literal, size);

	return &(nexp->self);
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

static void parseVariableDeclaration(struct VariableDecl* decl){

	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after var keyword in variable declaration");
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

	consume(TOKEN_SCOLON, "Expect semicolon at end of variable declaration");
}

static void parseSignalDeclaration(struct SignalDecl* decl){

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
}

static void parseVariableAssignment(struct VariableAssign *varAssign){
#ifdef DEBUG
	memcpy(&(varAssign->token), &(p->currToken), sizeof(struct Token));
#endif

	consume(TOKEN_IDENTIFIER, "expect identifer at start of statement");
	varAssign->target = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_VASSIGN, "Expect := after identifier");
	
	nextToken();
	varAssign->expression = parseExpression(LOWEST_PREC);

	consume(TOKEN_SCOLON, "Expect semicolon at end of variable assignment");	
}

static void parseSignalAssignment(struct SignalAssign *sigAssign){
#ifdef DEBUG
	memcpy(&(sigAssign->token), &(p->currToken), sizeof(struct Token));
#endif

	consume(TOKEN_IDENTIFIER, "expect identifer at start of statement");
	sigAssign->target = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_SASSIGN, "Expect <= after identifier");
	
	nextToken();
	sigAssign->expression = parseExpression(LOWEST_PREC);

	consume(TOKEN_SCOLON, "Expect semicolon at end of signal assignment");	
}

static void parseSequentialAssignment(struct SequentialStatement* seqStmt){
	//TODO: this may need some work depending on what we do with labels
	switch(p->peekToken.type){
		case TOKEN_SASSIGN: {
			seqStmt->type = QSIGNAL_ASSIGNMENT;
			parseSignalAssignment(&(seqStmt->as.signalAssignment));
			break;
		}

		case TOKEN_VASSIGN: {
			seqStmt->type = VARIABLE_ASSIGNMENT;
			parseVariableAssignment(&(seqStmt->as.variableAssignment));
			break;
		}

		default:
			error(p->peekToken.lineNumber, p->peekToken.literal, "Expect valid assignment type in assignment statement");
			break;
	}
}

//need this forward declaration because sequentials can nest
static Dba* parseSequentialStatements();

static void parseWhileStatement(struct WhileStatement* wStmt){
#ifdef DEBUG
	memcpy(&(wStmt->token), &(p->currToken), sizeof(struct Token));
#endif

	consume(TOKEN_WHILE, "Expect token while at start of while statement");
	consumeNext(TOKEN_LPAREN, "Expect '(' after while token");	

	nextToken();
	if(!match(TOKEN_RPAREN)){
		//handle condition
		wStmt->condition = parseExpression(LOWEST_PREC);
	}
	consume(TOKEN_RPAREN, "Expect ')' after while condition");
	consumeNext(TOKEN_LBRACE, "Expect '{' at start of while body");

	nextToken();
	if(!match(TOKEN_RBRACE)){
		wStmt->statements = parseSequentialStatements();
	}

	consume(TOKEN_RBRACE, "expect '}' at end of process statement");
}

static void parseWaitStatement(struct WaitStatement* wStmt){
#ifdef DEBUG
	memcpy(&(wStmt->token), &(p->currToken), sizeof(struct Token));
#endif

	consume(TOKEN_WAIT, "Expect token wait at start of wait statement");
	nextToken();
	
	//TODO: add support for wait statements other than "wait;"

	consume(TOKEN_SCOLON, "Expect semicolon at end of wait statement");	
}

static Dba* parseSequentialStatements(){
	Dba* stmts = InitBlockArray(sizeof(struct SequentialStatement));
	
	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		
		struct SequentialStatement seqStmt = {0};
		
		switch(p->currToken.type){
			case TOKEN_IDENTIFIER: { 
				parseSequentialAssignment(&seqStmt);
				break;
			}

			case TOKEN_WAIT: {
				seqStmt.type = WAIT_STATEMENT;
				parseWaitStatement(&(seqStmt.as.waitStatement));
				break;
			}
	
			case TOKEN_WHILE: {
				seqStmt.type = WHILE_STATEMENT;
				parseWhileStatement(&(seqStmt.as.whileStatement));
				break;
			}
	
			default:
				error(p->currToken.lineNumber, p->currToken.literal, "Expect valid sequential statement");
				break;
		}

		WriteBlockArray(stmts, (char*)(&seqStmt));
		nextToken();	
	}

	return stmts;
}

static Dba* parseProcessBodyDeclarations(){
	Dba* decls = InitBlockArray(sizeof(struct Declaration));

	while(thereAreDeclarations()){
		
		struct Declaration decl = {0};

		switch(p->currToken.type){
			case TOKEN_SIG: {
				decl.type = SIGNAL_DECLARATION;
				parseSignalDeclaration(&(decl.as.signalDeclaration));
				break;
			}

			case TOKEN_VAR: {
				decl.type = VARIABLE_DECLARATION;
				parseVariableDeclaration(&(decl.as.variableDeclaration));
				break;
			}

			default:
				error(p->currToken.lineNumber, p->currToken.literal, "Expect valid declaration in process body");
				break;
		}

		WriteBlockArray(decls, (char*)(&decl));
		nextToken();	
	}

	return decls;
}

static void parseProcessStatement(struct Process* proc){
#ifdef DEBUG
	memcpy(&(proc->token), &(p->currToken), sizeof(struct Token));
#endif

	consume(TOKEN_PROC, "expect proc keyword at start of process");
	consumeNext(TOKEN_LPAREN, "expect '(' after proc keyword");
	
	if(peek(TOKEN_IDENTIFIER)){
		consumeNext(TOKEN_IDENTIFIER, "expect identifer in sensitivity list");
		proc->sensitivityList = (struct Identifier*)parseIdentifier();
	}
	
	consumeNext(TOKEN_RPAREN, "expect ')' after proc sensitivity list");
	consumeNext(TOKEN_LBRACE, "expect '{' after proc sensitivity list");

	nextToken();
	if(!match(TOKEN_RBRACE)){
		proc->declarations = parseProcessBodyDeclarations();	
		proc->statements = parseSequentialStatements();	
	}

	consume(TOKEN_RBRACE, "expect '}' at end of process statement");
}

static Dba* parseArchBodyStatements(){
	Dba* stmts = InitBlockArray(sizeof(struct ConcurrentStatement));
	
	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		
		struct ConcurrentStatement conStmt = {0};
		
		switch(p->currToken.type){
			case TOKEN_IDENTIFIER: { 
				//TODO: this may need some work depending on what we do with labels
				conStmt.type = SIGNAL_ASSIGNMENT;
				parseSignalAssignment(&(conStmt.as.signalAssignment));
				break;
			}
	
			case TOKEN_PROC: {
				conStmt.type = PROCESS;
				parseProcessStatement(&(conStmt.as.process));
				break;
			}
	
			default:
				error(p->currToken.lineNumber, p->currToken.literal, "Expect valid concurrent statement in architecture body");
				break;
		}

		WriteBlockArray(stmts, (char*)(&conStmt));
		nextToken();	
	}

	return stmts;
}

static Dba* parseArchBodyDeclarations(){
	Dba* decls = InitBlockArray(sizeof(struct Declaration));

	while(thereAreDeclarations()){
		
		struct Declaration decl = {0};

		switch(p->currToken.type){
			case TOKEN_SIG: {
				decl.type = SIGNAL_DECLARATION;
				parseSignalDeclaration(&(decl.as.signalDeclaration));
				break;
			}

			default:
				error(p->currToken.lineNumber, p->currToken.literal, "Expect valid concurrent statement in architecture body");
				break;
		}

		WriteBlockArray(decls, (char*)(&decl));
		nextToken();	
	}

	return decls;
}

static Dba* parsePortDecl(){
	Dba* ports = InitBlockArray(sizeof(struct PortDecl)); 	

	nextToken();

	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		struct PortDecl port;		

		consume(TOKEN_IDENTIFIER, "Expect identifier at start of port declaration");
		port.name = (struct Identifier*)parseIdentifier();
		
		nextToken();
		if(!match(TOKEN_INPUT) && !match(TOKEN_OUTPUT) && !match(TOKEN_INOUT)){
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid port mode");
		}	
		port.pmode = parsePortMode(p->currToken.literal);
		
		nextToken();
		if(!validDataType()){
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid data type");
		}	
		port.dtype = parseDataType(p->currToken.literal);
		
		consumeNext(TOKEN_SCOLON, "Expect ; at end of port declaration");
		WriteBlockArray(ports, (char*)(&port));
		
		nextToken();
	}

	return ports;
}

static void parseArchitectureDecl(struct ArchitectureDecl* aDecl){
#ifdef DEBUG
	memcpy(&(aDecl->token), &(p->currToken), sizeof(struct Token));
#endif
	
	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after keyword arch");
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

static struct UseStatement parseUseStatement(){
	struct UseStatement stmt = {0};

#ifdef DEBUG	
	memcpy(&(stmt.token), &(p->currToken), sizeof(struct Token));
#endif

	consumeNext(TOKEN_IDENTIFIER, "Expect use path after use keyword");
	
	int size = strlen(p->currToken.literal) + 1;
	stmt.value = malloc(sizeof(char) * size);
	memcpy(stmt.value, p->currToken.literal, size);
	
	consumeNext(TOKEN_SCOLON, "Expect ; at end of use statment");

	return stmt;
}

static struct DesignUnit parseDesignUnit(){
	struct DesignUnit unit = {0};

	switch(p->currToken.type){
		case TOKEN_ENT: { 
			unit.type = ENTITY;
			parseEntityDecl(&(unit.as.entity));
			break;
		}

		case TOKEN_ARCH: {
			unit.type = ARCHITECTURE;
			parseArchitectureDecl(&(unit.as.architecture));
			break;
		}

		default:
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid design unit type");
			break;
	}

	return unit;
}

struct Program* ParseProgram(){
	struct Program* prog = calloc(1, sizeof(struct Program));

	// first get the use statements
	while(p->currToken.type == TOKEN_USE && p->currToken.type != TOKEN_EOP){
		struct UseStatement stmt = parseUseStatement();
	
		if(prog->useStatements == NULL){
			prog->useStatements = InitBlockArray(sizeof(struct UseStatement));
		}
		WriteBlockArray(prog->useStatements, (char*)(&stmt));
		nextToken();
	}

	// next parse any design units
	while(p->currToken.type != TOKEN_EOP){
		struct DesignUnit unit = parseDesignUnit();
		
		if(prog->units == NULL){
			prog->units = InitBlockArray(sizeof(struct DesignUnit));	
		}
		WriteBlockArray(prog->units, (char*)(&unit));
		nextToken();
	}
	
	return prog;
}
