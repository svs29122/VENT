#ifndef INC_UTILS_H
#define INC_UTILS_H

static struct Token nextToken();

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

static bool thereAreDeclarations(){
	bool valid = false;

	valid = 	match(TOKEN_SIG)		|| match(TOKEN_VAR)		||
				match(TOKEN_COMP)		|| match(TOKEN_FILE);

	return valid;
}

#endif // INC_UTILS_H
