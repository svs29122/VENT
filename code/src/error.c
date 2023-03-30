#include <stdio.h>
#include <stdbool.h>

bool hadError;

void error(int line, char* where, const char* message){
	fprintf(stderr, "[line %d] Error at \'%s\': %s\n", line, where, message);
	hadError = true;
} 
