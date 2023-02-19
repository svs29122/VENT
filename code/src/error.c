#include <stdio.h>
#include <stdbool.h>

bool hadError;

void report(int line, char* where, char* message){
	printf("[line %d] Error%s: %s\n", line, where, message);
	hadError = true;
} 

void error(int line, char* message){
	report(line, "", message);
}

