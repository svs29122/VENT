#ifndef INC_ERROR_H
#define INC_ERROR_H

bool hadError;

static void error(int line, char* where, const char* message){
	fprintf(stderr, "\e[0;31m[line %d] Error at \'%s\': %s\n\e[0m", line, where, message);
	hadError = true;
}


#endif //INC_ERROR_H
