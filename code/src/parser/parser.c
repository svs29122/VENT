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

//private includes
#include "internal_parser.h"
#include "rules.h"
#include "error.h"
#include "utils.h"
#include "expression.h"

static struct parser parser;
struct parser *p = &parser;

struct DynamicBlockArray* componentStore;
struct DynamicHashTable* enumTypeTable;

void SetPrintTokenFlag(){
	p->printTokenFlag = true;
}

static void initParser(){

	//preserve printToken btw inits
	bool keepPrinting = p->printTokenFlag;

	memset(p, 0, sizeof(struct parser));

	p->printTokenFlag = keepPrinting;
	p->currToken = NextToken();
	p->peekToken = NextToken();

	componentStore = InitBlockArray(sizeof(struct Declaration));
	enumTypeTable = InitHashTable();
}

void nextToken(){	
	
	if(p->printTokenFlag) PrintToken(p->currToken);

	free(p->currToken.literal);

	p->currToken = p->peekToken;
	p->peekToken = NextToken();
}

static struct ParseRule* getRule(enum TOKEN_TYPE type){
	return &rules[type];
}

static struct Expression* parseExpression(enum Precedence precedence){
	ParsePrefixFn prefixRule = getRule(p->currToken.type)->prefix;

	if(prefixRule == NULL){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect l-value expression");
		return NULL;
	}	
	struct Expression* leftExp = prefixRule();

	//never move past the semicolon at the end of an expression
	if(!match(TOKEN_SCOLON)) nextToken();	

	while(!peek(TOKEN_SCOLON) && precedence < getRule(p->currToken.type)->precedence){
		ParseInfixFn infixRule = getRule(p->currToken.type)->infix;

		if(infixRule == NULL){
			return leftExp;
		}

		leftExp = infixRule(leftExp);
	}

	return leftExp;
}

static struct Expression* parseIdentifier(){
	struct Identifier* ident = calloc(1, sizeof(struct Identifier));
#ifdef DEBUG
	memcpy(&(ident->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	ident->self.root.type = AST_IDENTIFIER;
	ident->self.type = NAME_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	ident->value = calloc(size, sizeof(char));
	memcpy(ident->value, p->currToken.literal, size);
	
	return &(ident->self);
}

static struct Expression* parseCharLiteral(){
	struct CharExpr* chexp = calloc(1, sizeof(struct CharExpr));
#ifdef DEBUG
	memcpy(&(chexp->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	chexp->self.root.type = AST_EXPRESSION;
	chexp->self.type = CHAR_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	chexp->literal = calloc(size, sizeof(char));
	memcpy(chexp->literal, p->currToken.literal, size);
	
	return &(chexp->self);
}

static struct Expression* parseStringLiteral(){
	struct StringExpr* stexp = calloc(1, sizeof(struct StringExpr));
#ifdef DEBUG
	memcpy(&(stexp->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	stexp->self.root.type = AST_EXPRESSION;
	stexp->self.type = STRING_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	stexp->literal = calloc(size, sizeof(char));
	memcpy(stexp->literal, p->currToken.literal, size);
	
	return &(stexp->self);
}

static struct Expression* parseNumericLiteral(){
	struct NumExpr* nexp = calloc(1, sizeof(struct NumExpr));
#ifdef DEBUG
	memcpy(&(nexp->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	nexp->self.root.type = AST_EXPRESSION;
	nexp->self.type = NUM_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	nexp->literal = calloc(size, sizeof(char));
	memcpy(nexp->literal, p->currToken.literal, size);

	return &(nexp->self);
}

static struct Expression* parseUnary(){
	struct UnaryExpr* uexp = calloc(1, sizeof(struct UnaryExpr));
#ifdef DEBUG
	memcpy(&(uexp->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	uexp->self.root.type = AST_EXPRESSION;
	uexp->self.type = UNARY_EXPR;

	int size = strlen(p->currToken.literal) + 1;
	uexp->op = calloc(size, sizeof(char));
	memcpy(uexp->op, p->currToken.literal, size);

	enum Precedence precedence = getRule(p->currToken.type)->precedence;
	nextToken();

	uexp->right = parseExpression(precedence);

	return &(uexp->self);
}

static struct Expression* parseBinary(struct Expression* expr){
	struct BinaryExpr* biexp = calloc(1, sizeof(struct BinaryExpr));
#ifdef DEBUG
	memcpy(&(biexp->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	biexp->self.root.type = AST_EXPRESSION;

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

static struct Expression* parseAttribute(struct Expression* expr){
	struct AttributeExpr* atexp = calloc(1, sizeof(struct AttributeExpr));
#ifdef DEBUG
	memcpy(&(atexp->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	atexp->self.root.type = AST_EXPRESSION;

	atexp->self.type = ATTRIBUTE_EXPR;
	atexp->object = expr;

	atexp->tick = '\'';

	enum Precedence precedence = getRule(p->currToken.type)->precedence;
	nextToken();
	
	atexp->attribute = parseExpression(precedence);

	return &(atexp->self);
}

static struct Expression* parseCall(struct Expression* expr){
	struct CallExpr* cexp = calloc(1, sizeof(struct CallExpr));
#ifdef DEBUG
	memcpy(&(cexp->self.root.token), &(p->currToken), sizeof(struct Token));
#endif
	cexp->self.root.type = AST_EXPRESSION;

	cexp->self.type = CALL_EXPR;
	cexp->function = expr;

	consume(TOKEN_LPAREN, "Expect '(' after function in call expression");

	enum Precedence precedence = getRule(p->currToken.type)->precedence;
	nextToken();
	
	cexp->parameters = parseExpression(precedence);

	consume(TOKEN_RPAREN, "Expect ')' after paremeter in call expression");
	nextToken();

	return &(cexp->self);
}

static struct Label* parseLabel(){
	
	if(match(TOKEN_IDENTIFIER) && peek(TOKEN_COLON)){
		
		//we got a label
		struct Label* label = calloc(1, sizeof(struct Label));
		label->self.type = AST_LABEL;
		
		int size = strlen(p->currToken.literal) + 1;
		label->value = calloc(size, sizeof(char));
		memcpy(label->value, p->currToken.literal, size);
		
		//step past label name and colon
		nextToken();
		nextToken();

		return label;
	}	

	return NULL;
}

static struct Range* parseRange(){
	struct Range* rng = calloc(1, sizeof(struct Range));
	rng->self.type = AST_RANGE;
	
	rng->left = parseExpression(LOWEST_PREC);

	if(match(TOKEN_TO) || match(TOKEN_DOWNTO)){
		if(match(TOKEN_DOWNTO)){
			rng->descending = true;
		}
	
		nextToken();
		rng->right = parseExpression(LOWEST_PREC);	
	}

	return rng;
}

static char* parseAssignmentOperator(){
	int len = 3; // two chars  + \0
	char* op = calloc(len, sizeof(char));
	
	memcpy(op, p->currToken.literal, len-1); 

	return op;
}

static struct DataType* parseDataType(char* val){
	struct DataType* dt = calloc(1, sizeof(struct DataType));
#ifdef DEBUG
	memcpy(&(dt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	dt->self.type = AST_DTYPE;

	int size = strlen(val) + 1;
	dt->value = calloc(size, sizeof(char));
	memcpy(dt->value, val, size);

	if(peek(TOKEN_LPAREN)){
		consumeNext(TOKEN_LPAREN, "Expect '(' before range in data type");

		nextToken();
		dt->range = parseRange();

		consume(TOKEN_RPAREN, "Expect ')' after range in data type");
	}

	return dt;
}

static struct PortMode* parsePortMode(char* val){
	struct PortMode* pm = calloc(1, sizeof(struct PortMode));
#ifdef DEBUG
	memcpy(&(pm->self.token), &(p->currToken), sizeof(struct Token));
#endif
	pm->self.type = AST_PMODE;

	int size = strlen(val) +1;
	pm->value = calloc(size, sizeof(char));
	memcpy(pm->value, val, size);

	return pm;
}

static void parseVariableDeclaration(struct VariableDecl* decl){
	decl->self.type = AST_VDECL;

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

struct ExpressionNode* parseEnumerationList(){

	struct ExpressionNode* elist = calloc(1, sizeof(struct ExpressionNode));
	struct ExpressionNode *currList = elist;
	
	while(!match(TOKEN_RBRACE)){
				
		struct Token prevToken = copyToken(p->currToken);

		struct Expression* curr = parseExpression(LOWEST_PREC);
		if(curr == NULL) return NULL;

		if(curr->type != NAME_EXPR && curr->type != CHAR_EXPR){
			error(prevToken.lineNumber, prevToken.literal, "Expect only identifier or char literal in type enumeration list");
		} else {
			currList->expression = curr;
		}
		destroyToken(prevToken);
	
		if(!match(TOKEN_RBRACE)) {		
			consume(TOKEN_COMMA, "Expect comma after expression in expression list");
			nextToken();

			currList->next = calloc(1, sizeof(struct ExpressionNode));
			currList = currList->next; 
		}
	}
		
	return elist;
}

static void parseTypeDeclaration(struct TypeDecl* decl){
	decl->self.type = AST_TDECL;

	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after type keyword in type declaration");
	decl->typeName = (struct Identifier*)parseIdentifier();
	consumeNext(TOKEN_LBRACE, "Expect { after typeName in type declaration");

	nextToken();
	if(match(TOKEN_RBRACE)){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect non-empty enumeration list in type declaration");
	} else {
		decl->enumList = parseEnumerationList();
	}

	//add this new type to the enumType lookup table
	SetInHashTable(enumTypeTable, decl->typeName->value, 1);

	consume(TOKEN_RBRACE, "Expect } after last enum in type declaration");
	consumeNext(TOKEN_SCOLON, "Expect semicolon at end of type declaration");
}

static void parseSignalDeclaration(struct SignalDecl* decl){
	decl->self.type = AST_SDECL;

	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after sig keyword in signal declaration");
	decl->name = (struct Identifier*)parseIdentifier();
		
	nextToken();
	if(validDataType() || userDefinedDataType()){
		decl->dtype = parseDataType(p->currToken.literal);
	} else {
		error(p->currToken.lineNumber, p->currToken.literal, "Expect valid data type after signal identifier");
	}
		
	nextToken();
	if(match(TOKEN_VASSIGN)){
		nextToken();
		decl->expression = parseExpression(LOWEST_PREC);	
	}

	consume(TOKEN_SCOLON, "Expect semicolon at end of signal declaration");
}

static void parseComponentInterior(struct ComponentDecl *cDecl){

	nextToken();
	uint16_t posInComponent = 1;

	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		if(thisIsAPort()) {
			struct PortDecl port = parsePortDecl();	
			port.position = posInComponent++;

			if(cDecl->ports == NULL) {
				cDecl->ports = InitBlockArray(sizeof(struct PortDecl)); 	
			}

			WriteBlockArray(cDecl->ports, (char*)(&port));
		} else { //this is a generic
			struct GenericDecl generic = parseGenericDecl();	
			generic.position = posInComponent++;

			if(cDecl->generics == NULL) {
				cDecl->generics = InitBlockArray(sizeof(struct GenericDecl)); 	
			}

			WriteBlockArray(cDecl->generics, (char*)(&generic));
		}
		nextToken();
	}
}

static void parseComponentDeclaration(struct ComponentDecl* cDecl){
#ifdef DEBUG
	memcpy(&(cDecl->self.token), &(p->currToken), sizeof(struct Token));
#endif
	cDecl->self.type = AST_COMPONENT;
	
	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after comp keyword");
	cDecl->name = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_LBRACE, "Expect { after component identifier");

	if(!peek(TOKEN_RBRACE)){
		parseComponentInterior(cDecl);
	} else {
		nextToken();
	}

	consume(TOKEN_RBRACE, "Expect } at the end of component declaration");
}

static void parseVariableAssignment(struct VariableAssign *varAssign){
#ifdef DEBUG
	memcpy(&(varAssign->self.token), &(p->currToken), sizeof(struct Token));
#endif
	varAssign->self.type = AST_VASSIGN;

	consume(TOKEN_IDENTIFIER, "expect identifer at start of statement");
	varAssign->target = (struct Identifier*)parseIdentifier();
	
	nextToken();
	if(!validAssignment()){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect valid variable assignment operator");
	}
	varAssign->op = parseAssignmentOperator();
	
	if(!match(TOKEN_PLUS_PLUS) && !match(TOKEN_MINUS_MINUS)){
		nextToken();
		varAssign->expression = parseExpression(LOWEST_PREC);
	} else {
		nextToken();
	}

	consume(TOKEN_SCOLON, "Expect semicolon at end of variable assignment");	
}

static void parseSignalAssignment(struct SignalAssign *sigAssign){
	sigAssign->self.type = AST_SASSIGN;

	consume(TOKEN_IDENTIFIER, "expect identifer at start of statement");
	sigAssign->target = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_LESS_EQUAL, "Expect <= after identifier");
#ifdef DEBUG
	memcpy(&(sigAssign->self.token), &(p->currToken), sizeof(struct Token));
#endif
	
	nextToken();
	sigAssign->expression = parseExpression(LOWEST_PREC);

	consume(TOKEN_SCOLON, "Expect semicolon at end of signal assignment");	
}

static void parseSequentialAssignment(struct SequentialStatement* seqStmt){
	//TODO: this may need some work depending on what we do with labels
	switch(p->peekToken.type){
		case TOKEN_LESS_EQUAL: {
			seqStmt->type = QSIGNAL_ASSIGNMENT;
			parseSignalAssignment(&(seqStmt->as.signalAssignment));
			break;
		}

		case TOKEN_PLUS_EQUAL:
		case TOKEN_PLUS_PLUS:
		case TOKEN_MINUS_EQUAL:
		case TOKEN_MINUS_MINUS:
		case TOKEN_STAR_EQUAL:
		case TOKEN_SLASH_EQUAL:
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

static struct Choice* parseCaseChoices(){
	struct Choice* listOfChoices = calloc(1, sizeof(struct Choice));	
	struct Choice* choice = listOfChoices;

	while(!match(TOKEN_COLON)){
		if(peek(TOKEN_TO) || peek(TOKEN_DOWNTO)){
			choice->type = CHOICE_RANGE;
			choice->as.range = parseRange();	
		} else {
			if(match(TOKEN_BAR)){
				choice->nextChoice = calloc(1, sizeof(struct Choice));
				choice = choice->nextChoice;
			} else {
				choice->as.numExpr = parseNumericLiteral();
			}
			nextToken();
		}
	}

	return listOfChoices;
}

//need this forward declaration because sequentials can nest
static Dba* parseSequentialStatements();

static Dba* parseCaseStatements(){
	Dba* cstmts = InitBlockArray(sizeof(struct CaseStatement));

	while(match(TOKEN_CASE) || match(TOKEN_DEFAULT)){
		struct CaseStatement caseStmt = {0};
#ifdef DEBUG
		memcpy(&(caseStmt.self.token), &(p->currToken), sizeof(struct Token));
#endif
		caseStmt.self.type = AST_CASE;
	
		
		if(match(TOKEN_CASE)){
			consume(TOKEN_CASE, "Expect token case at start of case statement");
		} else {
			consume(TOKEN_DEFAULT, "Expect token default if no token case in case statement");
			caseStmt.defaultCase = true;
		}

		nextToken();
		if(!match(TOKEN_COLON)){
			//TODO: flag error when case statement has no choice before colon?
			caseStmt.choices = parseCaseChoices();
		}
		consume(TOKEN_COLON, "Expect colon after choices in case statement");

		nextToken();
		caseStmt.statements = parseSequentialStatements();
		
		WriteBlockArray(cstmts, (char*)(&caseStmt)); 
	}

	return cstmts;
}

static void parseSwitchStatement(struct SwitchStatement* switchStmt){
#ifdef DEBUG
	memcpy(&(switchStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	switchStmt->self.type = AST_SWITCH;
	
	consume(TOKEN_SWITCH, "Expect token switch at start of switch statement");
	consumeNext(TOKEN_LPAREN, "Expect '(' after if token");	

	nextToken();
	if(match(TOKEN_RPAREN)){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect valid expression in switch  statement");
	} else {
		switchStmt->expression = parseExpression(LOWEST_PREC);
	}
	consume(TOKEN_RPAREN, "Expect ')' after switch expression");
	consumeNext(TOKEN_LBRACE, "Expect '{' at start of switch statement body");

	nextToken();
	if(!match(TOKEN_RBRACE)){
		switchStmt->cases = parseCaseStatements();
	}
	consume(TOKEN_RBRACE, "expect '}' at end of if statement");
};

static void parseIfStatement(struct IfStatement* ifStmt){
#ifdef DEBUG
	memcpy(&(ifStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	if(ifStmt->inElsIf == true){
		ifStmt->self.type = AST_ELSIF;	
	} else {
		ifStmt->self.type = AST_IF;
	}

	if(ifStmt->inElsIf == true){
		consume(TOKEN_ELSIF, "Expect token elsif at start of elsif statement");
	} else {
		consume(TOKEN_IF, "Expect token if at start of if statement");
	}
	consumeNext(TOKEN_LPAREN, "Expect '(' after if token");	

	nextToken();
	if(match(TOKEN_RPAREN)){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect valid antecedent in if statement");
	} else {
		ifStmt->antecedent = parseExpression(LOWEST_PREC);
	}
	consume(TOKEN_RPAREN, "Expect ')' after if antecedent");
	consumeNext(TOKEN_LBRACE, "Expect '{' at start of if statement body");

	nextToken();
	if(!match(TOKEN_RBRACE)){
		ifStmt->consequentStatements = parseSequentialStatements();
	}
	consume(TOKEN_RBRACE, "expect '}' at end of if statement");

	//check for elsif block
	if(peek(TOKEN_ELSIF)){
		ifStmt->elsif = calloc(1, sizeof(struct IfStatement));
		ifStmt->elsif->inElsIf = true;		

		nextToken();
		parseIfStatement(ifStmt->elsif);
	}

	// check for else block
	if(!ifStmt->inElsIf && peek(TOKEN_ELSE)){
		consumeNext(TOKEN_ELSE, "Expect else token");
		consumeNext(TOKEN_LBRACE, "Expect '{' after else token");	
		
		nextToken();
		if(!match(TOKEN_RBRACE)){
			ifStmt->alternativeStatements = parseSequentialStatements();
		}
		consume(TOKEN_RBRACE, "expect '}' at end of else statement");
	}
}

static void parseForStatement(struct ForStatement* fStmt){
#ifdef DEBUG
	memcpy(&(fStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	fStmt->self.type = AST_FOR;	

	consume(TOKEN_FOR, "Expect token for at start of for statement");
	consumeNext(TOKEN_LPAREN, "Expect '(' after for token");	

	consumeNext(TOKEN_IDENTIFIER, "Expect identifier in for loop paraemeter");
	fStmt->parameter = (struct Identifier*)parseIdentifier();
	consumeNext(TOKEN_COLON, "Expect colon after paramter in for statement");

	nextToken();
	fStmt->range = parseRange();
	
	consume(TOKEN_RPAREN, "Expect ')' after range in for statement");
	consumeNext(TOKEN_LBRACE, "Expect '{' at start of for body");

	nextToken();
	if(!match(TOKEN_RBRACE)){
		fStmt->statements = parseSequentialStatements();
	}

	consume(TOKEN_RBRACE, "expect '}' at end of process statement");
}

static void parseNullStatement(struct NullStatement* nStmt){
#ifdef DEBUG
	memcpy(&(nStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	nStmt->self.type = AST_NULL;

	consume(TOKEN_NULL, "Expect token null at start of null statement");

	consumeNext(TOKEN_SCOLON, "Expect semicolon at end of null statement");	
}

static struct SeverityStatement parseSeverityStatement(){
	struct SeverityStatement sevStmt = {0};

	nextToken();
	switch(p->currToken.type){
		case TOKEN_NOTE:
			sevStmt.level = SEVERITY_NOTE;
			break;

		case TOKEN_WARNING:
			sevStmt.level = SEVERITY_WARNING;
			break;

		case TOKEN_ERROR:
			sevStmt.level = SEVERITY_ERROR;
			break;

		case TOKEN_FAILURE:
			sevStmt.level = SEVERITY_FAILURE;
			break;

		default:
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid Severity level in severity statement");
			break;
	}	
	
	return sevStmt;
}

static void parseReportStatement(struct ReportStatement* rStmt){
#ifdef DEBUG
	memcpy(&(rStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	rStmt->self.type = AST_REPORT;

	consume(TOKEN_REPORT, "Expect token report at start of report statement");

	nextToken();
	if(!match(TOKEN_STRINGLIT)){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect string literalin report statement");
	}
	rStmt->stringExpr = parseStringLiteral();

	nextToken();
	if(match(TOKEN_SEVERITY)){
		rStmt->severity = parseSeverityStatement();
		nextToken();
	}

	consume(TOKEN_SCOLON, "Expect semicolon at end of report statement");	
}

static void parseAssertStatement(struct AssertStatement * aStmt){
#ifdef DEBUG
	memcpy(&(aStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	aStmt->self.type = AST_ASSERT;

	consume(TOKEN_ASSERT, "Expect token assert at start of assert statement");
	consumeNext(TOKEN_LPAREN, "Expect '(' after assert token");	

	nextToken();
	if(match(TOKEN_RPAREN)){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect valid condition in assert statement");
	} else {
		aStmt->condition = parseExpression(LOWEST_PREC);
	}
	consume(TOKEN_RPAREN, "Expect ')' after assert condition");	

	nextToken();
	if(match(TOKEN_REPORT)){
		parseReportStatement(&(aStmt->report));
	}	

	consume(TOKEN_SCOLON, "Expect semicolon at end of assert statement");	
}

static void parseLoopStatement(struct LoopStatement* lStmt){
#ifdef DEBUG
	memcpy(&(lStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	lStmt->self.type = AST_LOOP;

	consume(TOKEN_LOOP, "Expect token loop at start of loop statement");
	consumeNext(TOKEN_LBRACE, "Expect '{' at start of while body");

	nextToken();
	if(!match(TOKEN_RBRACE)){
		lStmt->statements = parseSequentialStatements();
	}

	consume(TOKEN_RBRACE, "expect '}' at end of process statement");
}

static void parseWhileStatement(struct WhileStatement* wStmt){
#ifdef DEBUG
	memcpy(&(wStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	wStmt->self.type = AST_WHILE;

	consume(TOKEN_WHILE, "Expect token while at start of while statement");
	consumeNext(TOKEN_LPAREN, "Expect '(' after while token");	

	nextToken();
	if(!match(TOKEN_RPAREN)){
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
	memcpy(&(wStmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	wStmt->self.type = AST_WAIT;

	consume(TOKEN_WAIT, "Expect token wait at start of wait statement");
	nextToken();
	
	//TODO: add support for wait statements other than "wait;"

	consume(TOKEN_SCOLON, "Expect semicolon at end of wait statement");	
}

static Dba* parseSequentialStatements(){
	Dba* stmts = InitBlockArray(sizeof(struct SequentialStatement));
	
	while(!match(TOKEN_RBRACE) && !match(TOKEN_CASE) && !match(TOKEN_DEFAULT) && !match(TOKEN_EOP)){
		
		struct SequentialStatement seqStmt = {0};
		
		switch(p->currToken.type){
			case TOKEN_IDENTIFIER: { 
				parseSequentialAssignment(&seqStmt);
				break;
			}

			case TOKEN_IF: {
				seqStmt.type = IF_STATEMENT;
				parseIfStatement(&(seqStmt.as.ifStatement));
				break;
			}

			case TOKEN_FOR: {
				seqStmt.type = FOR_STATEMENT;
				parseForStatement(&(seqStmt.as.forStatement));
				break;
			}

			case TOKEN_LOOP: {
				seqStmt.type = LOOP_STATEMENT;
				parseLoopStatement(&(seqStmt.as.loopStatement));
				break;
			}

			case TOKEN_NULL: {
				seqStmt.type = NULL_STATEMENT;
				parseNullStatement(&(seqStmt.as.nullStatement));
				break;
			}

			case TOKEN_ASSERT: {
				seqStmt.type = ASSERT_STATEMENT;
				parseAssertStatement(&(seqStmt.as.assertStatement));
				break;
			}

			case TOKEN_REPORT: {
				seqStmt.type = REPORT_STATEMENT;
				parseReportStatement(&(seqStmt.as.reportStatement));
				break;
			}

			case TOKEN_SWITCH: {
				seqStmt.type = SWITCH_STATEMENT;
				parseSwitchStatement(&(seqStmt.as.switchStatement));
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
			// TODO: VHDL does not support SIGNAL declarations in process declaration zone need to remove this and fix tests
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

static void parseWildCardMap(struct ExpressionNode **portHead, struct Identifier* name){
	struct ExpressionNode *portMap = NULL, *pHead = NULL;
	struct ComponentDecl* comp = getComponentFromStore(name->value);

	//build the instance mappings from the component
	if(comp){
		//TODO: perhaps we should check the generic mappings to make sure either
		//a default value has been set up or an instance value has been passed in
		for(int i=0; i<BlockCount(comp->ports); i++){
			struct PortDecl* port = (struct PortDecl*)ReadBlockArray(comp->ports, i);
			if(portMap == NULL){
            portMap = calloc(1, sizeof(struct ExpressionNode));
            pHead = portMap;
         } else {
            portMap->next = calloc(1, sizeof(struct ExpressionNode));
            portMap = portMap->next;
         }
         portMap->expression = createBinaryExpression((struct Expression*) port->name, "=>", (struct Expression*) port->name);
		}
	}

	*portHead = pHead;
}

static struct Expression* parseGenericMap(struct Expression* map, struct Identifier* name, uint16_t pos){
	struct DynamicBlockArray* generics = NULL;
	struct ComponentDecl* comp = getComponentFromStore(name->value);

	if(comp) generics = comp->generics;

   if(generics){
      for(int i=0; i<BlockCount(generics); i++){
         struct GenericDecl* generic = (struct GenericDecl*)ReadBlockArray(generics, i); 
         if(positionalMapping(map)){
            if(generic->position == pos){
               struct Expression* expr = createBinaryExpression((struct Expression*) generic->name, "=>", map);
					freeExpression(map);
					return expr;
            }
         } else if (associativeMapping(map)) {
            struct BinaryExpr* bexp = (struct BinaryExpr*)map;
            struct Identifier* left = (struct Identifier*)bexp->left;
            if(strncmp(generic->name->value, left->value, sizeof(left->value)) == 0) {
					return map;
				}
			} else {
				printf("Error determining mapping! Map type == %d\r\n", map->type);
			}
      }   
   }   

	return NULL;
}

static struct Expression* parsePortMap(struct Expression* map, struct Identifier* name, uint16_t pos){
	struct DynamicBlockArray* ports = NULL;
	struct ComponentDecl* comp = getComponentFromStore(name->value);

	if(comp) ports = comp->ports;

   if(ports){
      for(int i=0; i<BlockCount(ports); i++){
         struct PortDecl* port = (struct PortDecl*)ReadBlockArray(ports, i); 
         if(positionalMapping(map)){
            if(port->position == pos){
               struct Expression* expr = createBinaryExpression((struct Expression*) port->name, "=>", map);
					freeExpression(map);
					return expr;
            }
         } else if (associativeMapping(map)) {
            struct BinaryExpr* bexp = (struct BinaryExpr*)map;
            struct Identifier* left = (struct Identifier*)bexp->left;
            if(strncmp(port->name->value, left->value, sizeof(left->value)) == 0) {
					return map;
            }
         } else {  
         	printf("Error determining mapping! Map type == %d\r\n", map->type);
			}
      }   
   }   

	return NULL;
}

static struct ExpressionNode* parseInstanceMappings(struct Instantiation* instance){
	struct ExpressionNode *portMap = NULL, *genericMap = NULL;
	struct ExpressionNode *portHead = NULL, *genericHead = NULL;

	uint16_t posInMap = 1;
	nextToken();

	while(!match(TOKEN_RPAREN) && !match(TOKEN_EOP)){

		struct Expression* mapping = parseExpression(LOWEST_PREC);
		
		if(!match(TOKEN_RPAREN)){
			consume(TOKEN_COMMA, "expect comma after identifier in mapping");		
			nextToken();
		}

	   if(thisIsAWildCard(mapping)){
			parseWildCardMap(&portHead, instance->name);
		} else if(thisIsAGenericMap(mapping, instance->name, posInMap)) {
			if(genericMap == NULL){
				genericMap = calloc(1, sizeof(struct ExpressionNode));
				genericHead = genericMap;
			} else {
				genericMap->next = calloc(1, sizeof(struct ExpressionNode));
				genericMap = genericMap->next;
			}
			genericMap->expression = parseGenericMap(mapping, instance->name, posInMap);
		} else { //this is a port map
			if(portMap == NULL){
				portMap = calloc(1, sizeof(struct ExpressionNode));
				portHead = portMap;
			} else {
				portMap->next = calloc(1, sizeof(struct ExpressionNode));
				portMap = portMap->next;
			}
			portMap->expression = parsePortMap(mapping, instance->name, posInMap);
		}
		posInMap++;
	}

	instance->genericMap = genericHead;
	instance->portMap = portHead;
}

static void parseInstantiation(struct Instantiation* instance){
	instance->self.type = AST_INSTANCE;

	consume(TOKEN_IDENTIFIER, "expect identifer at start of instance");
	instance->name = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_MAP, "Expect mapping after identifier in instance");
#ifdef DEBUG
	memcpy(&(instance->self.token), &(p->currToken), sizeof(struct Token));
#endif
	
	consumeNext(TOKEN_LPAREN, "Expect lparen after map keyword in instance");
	parseInstanceMappings(instance);
	consume(TOKEN_RPAREN, "Expect rparen after mappings in instance");

	consumeNext(TOKEN_SCOLON, "Expect semicolon at end of instantiation");
}

static void parseProcessStatement(struct Process* proc){
#ifdef DEBUG
	memcpy(&(proc->self.token), &(p->currToken), sizeof(struct Token));
#endif
	proc->self.type = AST_PROCESS;

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
		conStmt.label = parseLabel();
		
		switch(p->currToken.type){
			case TOKEN_PROC: {
				conStmt.type = PROCESS;
				parseProcessStatement(&(conStmt.as.process));
				break;
			}
	
			case TOKEN_IDENTIFIER: { 
				//some concurrent statements begin with identifiers
				switch(p->peekToken.type){
					case TOKEN_MAP: {
						conStmt.type = INSTANTIATION;
						parseInstantiation(&(conStmt.as.instantiation));
						break;
					}

					case TOKEN_LESS_EQUAL: {
						conStmt.type = SIGNAL_ASSIGNMENT;
						parseSignalAssignment(&(conStmt.as.signalAssignment));
						break;
					}
					
					default:
						error(p->currToken.lineNumber, p->currToken.literal, 
							"Expect valid concurrent statement in architecture body");
						break;
				}
				break;
			}
	
			default:
				error(p->currToken.lineNumber, p->currToken.literal, 
					"Expect valid concurrent statement in architecture body");
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
			case TOKEN_TYPE: {
				decl.type = TYPE_DECLARATION;
				parseTypeDeclaration(&(decl.as.typeDeclaration));
				break;
			}

			case TOKEN_SIG: {
				decl.type = SIGNAL_DECLARATION;
				parseSignalDeclaration(&(decl.as.signalDeclaration));
				break;
			}

			case TOKEN_COMP: {
				decl.type = COMPONENT_DECLARATION;
				parseComponentDeclaration(&(decl.as.componentDeclaration));
				WriteBlockArray(componentStore, (char*)(&decl));
				break;
			}

			default:
				error(p->currToken.lineNumber, p->currToken.literal, 
					"Expect valid declaration statement in architecture declarations");
				break;
		}

		WriteBlockArray(decls, (char*)(&decl));
		nextToken();	

	} // end while

	return decls;
}

static void parseArchitectureDecl(struct ArchitectureDecl* aDecl){
#ifdef DEBUG
	memcpy(&(aDecl->self.token), &(p->currToken), sizeof(struct Token));
#endif
	aDecl->self.type = AST_ARCHITECTURE;
	
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

static struct GenericDecl parseGenericDecl(){

	struct GenericDecl generic = {0};		
	generic.self.type = AST_GENERIC;

	consume(TOKEN_IDENTIFIER, "Expect identifier at start of generic declaration");
	generic.name = (struct Identifier*)parseIdentifier();
		
	nextToken();
	if(!validDataType()){
		error(p->currToken.lineNumber, p->currToken.literal, "Expect valid data type");
	}	
	generic.dtype = parseDataType(p->currToken.literal);

	nextToken();
	if(match(TOKEN_VASSIGN)){
		nextToken();
		generic.defaultValue = parseExpression(LOWEST_PREC);	
	}
	
	consume(TOKEN_SCOLON, "Expect ; at end of generic declaration");
		
	return generic;
}

static struct PortDecl parsePortDecl(){

	struct PortDecl port = {0};		
	port.self.type = AST_PORT;

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
		
	return port;
}

static void parseEntityInterior(struct EntityDecl *eDecl){

	nextToken();
	uint16_t posInEntity = 1;
	
	while(!match(TOKEN_RBRACE) && !match(TOKEN_EOP)){
		if(thisIsAPort()) {
			struct PortDecl port = parsePortDecl();	
			port.position = posInEntity++;

			if(eDecl->ports == NULL) {
				eDecl->ports = InitBlockArray(sizeof(struct PortDecl)); 	
			}

			WriteBlockArray(eDecl->ports, (char*)(&port));
		} else {
			struct GenericDecl generic = parseGenericDecl();	
			generic.position = posInEntity++;

			if(eDecl->generics == NULL) {
				eDecl->generics = InitBlockArray(sizeof(struct GenericDecl)); 	
			}

			WriteBlockArray(eDecl->generics, (char*)(&generic));
		}

		nextToken();
	}
}

static void parseEntityDecl(struct EntityDecl* eDecl){
#ifdef DEBUG
	memcpy(&(eDecl->self.token), &(p->currToken), sizeof(struct Token));
#endif
	eDecl->self.type = AST_ENTITY;
	
	consumeNext(TOKEN_IDENTIFIER, "Expect identifier after ent keyword");
	eDecl->name = (struct Identifier*)parseIdentifier();
	
	consumeNext(TOKEN_LBRACE, "Expect { after entity identifier");

	if(!peek(TOKEN_RBRACE)){
		parseEntityInterior(eDecl);
	} else {
		nextToken();
	}

	consume(TOKEN_RBRACE, "Expect } at the end of entity declaration");
}

static void parseUseStatement(struct UseStatement* stmt){
#ifdef DEBUG	
	memcpy(&(stmt->self.token), &(p->currToken), sizeof(struct Token));
#endif
	stmt->self.type = AST_USE;

	consumeNext(TOKEN_IDENTIFIER, "Expect use path after use keyword");
	
	int size = strlen(p->currToken.literal) + 1;
	stmt->value = malloc(sizeof(char) * size);
	memcpy(stmt->value, p->currToken.literal, size);
	
	consumeNext(TOKEN_SCOLON, "Expect ; at end of use statment");
}

static void parseLibraryUnit(struct LibraryUnit* libUnit){

	switch(p->currToken.type){
		case TOKEN_ENT: { 
			libUnit->type = ENTITY;
			parseEntityDecl(&(libUnit->as.entity));
			break;
		}

		case TOKEN_ARCH: {
			libUnit->type = ARCHITECTURE;
			parseArchitectureDecl(&(libUnit->as.architecture));
			break;
		}

		default:
			error(p->currToken.lineNumber, p->currToken.literal, "Expect valid library unit type");
			break;
	}
}

static struct DesignUnit parseDesignUnit(){
	struct DesignUnit unit = {0};

	if(match(TOKEN_USE)){
		unit.type = USE_STATEMENT;
		parseUseStatement(&(unit.as.useStatement));
	} else {
		unit.type = LIBRARY_UNIT;
		parseLibraryUnit(&(unit.as.libraryUnit));	
	} 

	return unit;
}

struct Program* ParseProgram(char* ventProgram){
	// do some setup
	InitLexer(ventProgram);
	initParser();

	struct Program* prog = calloc(1, sizeof(struct Program));
	prog->self.type = AST_PROGRAM;

	while(!endOfProgram()){
		if(thereAreDesignUnits()){
			struct DesignUnit unit = parseDesignUnit();
			
			if(prog->units == NULL){
				prog->units = InitBlockArray(sizeof(struct DesignUnit));	
			}
			WriteBlockArray(prog->units, (char*)(&unit));
			nextToken();
		}
	}
	
	return prog;
}

