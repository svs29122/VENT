#ifndef INC_AST_H
#define INC_AST_H

#include <stdbool.h>
#include <stdint.h>

#include "token.h"
#include "dba.h"

struct Program;
struct AstNode;
struct Expression;
struct ExpressionNode;

typedef void (*astNodeOpPtr) (struct AstNode*);
typedef void (*expOpPtr) (struct Expression*);
typedef void (*blkOpPtr) (struct DynamicBlockArray*);

struct OperationBlock {
	astNodeOpPtr doDefaultOp;
	astNodeOpPtr doOpenOp;
	astNodeOpPtr doCloseOp;
	astNodeOpPtr doSpecialOp;
	expOpPtr doExpressionOp;
	blkOpPtr doBlockArrayOp;
};

void WalkTree(struct Program* prog, struct OperationBlock* op);
uint16_t ExpressionCount(struct ExpressionNode* eNode);

enum AstNodeType {
	AST_PROGRAM = 1,
	AST_USE,
	AST_ENTITY,
	AST_COMPONENT,
	AST_ARCHITECTURE,
	AST_LABEL,
	AST_PORT,
	AST_GENERIC,
	AST_PROCESS,
	AST_INSTANCE,
	AST_FOR,
	AST_IF,
	AST_ELSIF,
	AST_LOOP,
	AST_NEXT,
	AST_EXIT,
	AST_RETURN,
	AST_NULL,
	AST_SWITCH,
	AST_CASE,
	AST_WAIT,
	AST_WHILE,
	AST_SASSIGN,
	AST_VASSIGN,
	AST_TDECL,
	AST_SDECL,
	AST_VDECL,
	AST_IDENTIFIER,
	AST_PMODE,
	AST_DTYPE,
	AST_RANGE,
	AST_EXPRESSION,
	AST_ASSERT,
	AST_REPORT,
};

struct AstNode {
#ifdef DEBUG
	struct Token token;
#endif
	enum AstNodeType type;
};

enum ExpressionType{
		BINARY_EXPR,
		UNARY_EXPR,
		GROUPED_EXPR,
		NAME_EXPR,
		NUM_EXPR,
		PHYLIT_EXPR,
		CHAR_EXPR,
		STRING_EXPR,
		AGGREGATE_EXPR,
		Q_EXPR,
		NEW_EXPR,
		CALL_EXPR,
};

struct Expression {
	struct AstNode root;
	enum ExpressionType type;
};

struct BinaryExpr {
	struct Expression self;
	struct Expression* left;
	struct Expression* right;
	char* op;
};

struct UnaryExpr {
	struct Expression self;
	struct Expression* right;
	char* op;
};

struct NameExpr {
	struct Expression self;
	struct Identifier* name;
};

struct CharExpr {
	struct Expression self;
	char* literal;
};

struct StringExpr {
	struct Expression self;
	char* literal;
};

struct NumExpr {
	struct Expression self;
	char* literal;
};

struct Identifier {
	struct Expression self;
	char* value;
	//TODO: perhaps identifiers can have pointers to other identifiers?
	//this would be an easy way to implement comma separated lists of 
	//identifiers e.g. in portDecls and proc sensitivity lists
};

struct Range {
	struct AstNode self;

	struct Expression* left;
	bool descending;
	struct Expression* right;
};

struct DataType {
	struct AstNode self;

	char* value;
	struct Range* range;
};

struct Label {
	struct AstNode self;

	char* value;
};

struct PortMode {
	struct AstNode self;

	char* value;
};

struct ExpressionNode {
	struct Expression* expression;
	struct ExpressionNode* next;
};

struct TypeDecl {
	struct AstNode self;
	
	struct Identifier* typeName;
	struct ExpressionNode *enumList;
}; 

struct VariableDecl {
	struct AstNode self;

	//TODO: need to add support for , separated identifier list
	struct Identifier *name;
	struct DataType* dtype;
	struct Expression* expression;
};

struct SignalDecl {
	struct AstNode self;

	//need to add support for , separated identifier list
	struct Identifier *name;
	struct DataType* dtype;
	struct Expression* expression;
};

struct ComponentDecl {
	struct AstNode self;

	struct Identifier* name;
	struct DynamicBlockArray* ports;
	struct DynamicBlockArray* generics;
};

struct Declaration {
	enum {
		//PROCEDURE_DECLARATION,
		//PROCEDURE_BODY,
		//FUNCTION_DECLARATION,
		//FUNCTION_BODY,
		TYPE_DECLARATION,
		//SUBTYPE_DECLARATION,
		//CONSTANT_DECLARATION,
		SIGNAL_DECLARATION,
		VARIABLE_DECLARATION,
		//FILE_DECLARATION,
		COMPONENT_DECLARATION,
	} type;
	union {
		struct TypeDecl typeDeclaration;
		struct SignalDecl signalDeclaration;
		struct VariableDecl variableDeclaration;
		struct ComponentDecl componentDeclaration;
	} as;
};

struct Choice {
	enum {
		CHOICE_NUMEXPR,
		CHOICE_RANGE,
	} type;
	union {
		struct Expression* numExpr;
		struct Range* range;
	} as;
	struct Choice* nextChoice;
};

struct CaseStatement {
	struct AstNode self;

	struct Choice* choices;
	bool defaultCase;
	struct DynamicBlockArray* statements;
};

struct SwitchStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* expression;
	struct DynamicBlockArray* cases;	
};

struct ForStatement {
	struct AstNode self;

	struct Label* label;
	struct Identifier* parameter;
	struct Range* range;
	struct DynamicBlockArray* statements;
};

struct IfStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* antecedent;
	
	struct DynamicBlockArray* consequentStatements;
	struct DynamicBlockArray* alternativeStatements;
	
	struct IfStatement* elsif;
	bool inElsIf;
};

struct LoopStatement {
	struct AstNode self;

	struct Label* label;
	struct DynamicBlockArray* statements;
};

struct NextStatement {
	struct AstNode self;

	struct Label* label;
	struct Label* loopLabel;
	struct Expression* condition;
};

struct ExitStatement {
	struct AstNode self;

	struct Label* label;
	struct Label* loopLabel;
	struct Expression* condition;
};

struct NullStatement {
	struct AstNode self;

	struct Label* label;
};

struct ReturnStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* expression;
};

struct WhileStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* condition;
	struct DynamicBlockArray* statements;
};

struct WaitStatement {
	struct AstNode self;

	struct Label* label;
	struct Identifier* sensitivityList;
	struct Expression* condition;
	struct Expression* time;
};

struct SeverityStatement{
	enum {
		SEVERITY_NULL = 0,
		SEVERITY_NOTE,
		SEVERITY_WARNING,
		SEVERITY_ERROR,
		SEVERITY_FAILURE,
	} level;
};

struct ReportStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* stringExpr;
	struct SeverityStatement severity;
};

struct AssertStatement {
	struct AstNode self;

	struct Label* label;
	struct Expression* condition;
	struct ReportStatement report;
};

struct VariableAssign {
	struct AstNode self;

	struct Label* label;
	struct Identifier* target;
	char* op;
	struct Expression* expression;
};

struct SignalAssign {
	struct AstNode self;

	struct Identifier* target;
	struct Expression* expression;
};

struct SequentialStatement {
	enum {
		FOR_STATEMENT,
		IF_STATEMENT,
		LOOP_STATEMENT,
		NEXT_STATEMENT,
		EXIT_STATEMENT,
		RETURN_STATEMENT,
		NULL_STATEMENT,
		QSIGNAL_ASSIGNMENT,
		SWITCH_STATEMENT,
		VARIABLE_ASSIGNMENT,
		WAIT_STATEMENT,
		WHILE_STATEMENT,
		ASSERT_STATEMENT,
		REPORT_STATEMENT,
	} type;
	union {
		struct ForStatement forStatement;
		struct IfStatement ifStatement;
		struct LoopStatement loopStatement;
		struct NextStatement nextStatement;
		struct ExitStatement exitStatement;
		struct ReturnStatement returnStatement;
		struct NullStatement nullStatement;
		struct SignalAssign signalAssignment;
		struct SwitchStatement switchStatement;
		struct VariableAssign variableAssignment;
		struct WaitStatement waitStatement;
		struct WhileStatement whileStatement;
		struct AssertStatement assertStatement;
		struct ReportStatement reportStatement;
	} as;
};

struct Instantiation {
	struct AstNode self;
	
	struct Identifier* name;
	struct ExpressionNode* portMap;
	struct ExpressionNode* genericMap;
};

struct Process {
	struct AstNode self;

	struct Identifier* sensitivityList;
	struct DynamicBlockArray* declarations;
	struct DynamicBlockArray* statements;
};

struct ConcurrentStatement {
	struct Label* label;
	enum {
		PROCESS,
		INSTANTIATION,
		SIGNAL_ASSIGNMENT,
		//GENERATE,
		//ASSERT,
		//PROCEDURE_CALL,
		//BLOCK,
	} type;
	union {
		struct Process process;
		struct Instantiation instantiation;
		struct SignalAssign signalAssignment;
	} as;
};

struct ArchitectureDecl {
	struct AstNode self;

	struct Identifier* archName;
	struct Identifier* entName;
	struct DynamicBlockArray* declarations;
	struct DynamicBlockArray* statements;
};

struct GenericDecl {
	struct AstNode self;

	//need to add support for , separated identifier list
	uint16_t position;
	struct Identifier* name;
	struct DataType* dtype; 
	struct Expression* defaultValue;
};

struct PortDecl {
	struct AstNode self;

	//need to add support for , separated identifier list
	uint16_t position;
	struct Identifier* name;
	struct PortMode* pmode;
	struct DataType* dtype; 
};

struct EntityDecl {
	struct AstNode self;

	struct Identifier* name;
	struct DynamicBlockArray* ports;
	struct DynamicBlockArray* generics;
};

struct DesignUnit{
	enum {
		ENTITY,
		ARCHITECTURE,
		//PACKAGE,
		//PACKAGE_BODY,
		//CONFIGURATION
	} type;
	union {
		struct EntityDecl entity;
		struct ArchitectureDecl architecture;
		//PackageDecl;
		//PackageBodyDecl;
		//ConfigurationDecl; 
	} as;
};

struct UseStatement {
	struct AstNode self;

	char* value;
};

struct Program {
	struct AstNode self;

	struct DynamicBlockArray* useStatements;
	struct DynamicBlockArray* units;
};

#endif //INC_AST_H
