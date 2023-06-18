#include <stdio.h>

#include "error.h"

static bool hadError;

void error(int line, char* where, const char* message){
	fprintf(stderr, "\e[0;31m[line %d] Error at \'%s\': %s\n\e[0m", line, where, message);
	hadError = true;
}

bool ThereWasAnError(void){
	return hadError;
}
