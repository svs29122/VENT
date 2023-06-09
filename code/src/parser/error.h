#ifndef INC_ERROR_H
#define INC_ERROR_H

static bool hadError;

static void error(int line, char* where, const char* message){
	fprintf(stderr, "\e[0;31m[line %d] Error at \'%s\': %s\n\e[0m", line, where, message);
	hadError = true;
}

bool ThereWasAnError(void){
	return hadError;
}

#endif //INC_ERROR_H
