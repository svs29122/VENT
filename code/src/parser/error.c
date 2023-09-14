#include <stdio.h>

#include "error.h"

static bool hadError;

void error(int line, char* where, const char* message){
	if(!hadError){
		hadError = true;
		printf("\e[0;31m*** Got Errors ***\r\n");
	}
	fprintf(stderr, "\e[0;31m[line %d] Error at \'%s\': %s\n\e[0m", line, where, message);
}

bool ThereWasAnError(void){
	return hadError;
}

void resetErrors(void){
	hadError = 0;
}
