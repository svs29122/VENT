#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"
#include "parser.h"
#include "display.h"

static void doTranspile(char* fileName, bool printProgramTree);

int main(int argc, char* argv[]) {

	if(argc < 2){
		PrintUsage();
		return -1;
	}	

	bool skipTranspilation = false;
	bool printProgramTree = false;

	if(argc == 2){
		if(strcmp("-i", argv[1]) == 0){
			DoMenu();
			skipTranspilation = true;
		}
	} 
	if(argc == 3){
		if(strcmp("--print-tokens", argv[2]) == 0){
			SetPrintTokenFlag();
		} else if(strcmp("--print-ast", argv[2]) == 0){
			printProgramTree = true;
		}
	}
	
	if(!skipTranspilation){
		doTranspile(argv[1], printProgramTree);
	}

	return 0;
}

static char* readFile(const char* path){
	FILE* file = fopen(path, "rb");
	if(!file) {
		fprintf(stderr, "Unable to open file \"%s\".\n", path);
		exit(EXIT_FAILURE);
	}
	fseek(file, 0L, SEEK_END);
	size_t fileSize = ftell(file);
	rewind(file);

	char* buffer = (char*)malloc(fileSize + 1);
	if(!buffer){
		fprintf(stderr, "Unable to allocatate memory for reading \"%s\".\n", path);
		exit(EXIT_FAILURE);
	}

	size_t bytesRead = fread(buffer, sizeof(char), fileSize, file); 
	if(bytesRead < fileSize) {
		fprintf(stderr, "Unable to read file \"%s\".\n", path);
		exit(EXIT_FAILURE);
	}
	buffer[bytesRead-1] = '\0';

	fclose(file);
	return buffer;
}

extern bool hadError;

static void doTranspile(char* fileName, bool printProgramTree){
		char* ventSrc = readFile(fileName);
		InitLexer(ventSrc);
		InitParser();
		
		struct Program* prog = ParseProgram();
		if(printProgramTree){
			PrintProgram(prog);
		}
		
		printf("Transpilation complete");
		if(hadError){
			printf(" with errors");
		}
		printf("!\r\n");		

		FreeProgram(prog);
		free(ventSrc);
}
