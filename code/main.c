#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <parser.h>
#include <display.h>
#include <emitter.h>

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

static void doTranspile(char* fileName, bool printProgramTree, bool printTokens){
		char* ventSrc = readFile(fileName);
		
		if(printTokens) SetPrintTokenFlag();
		struct Program* prog = ParseProgram(ventSrc);

		if(printProgramTree) PrintProgram(prog);
		TranspileProgram(prog, fileName);

		printf("Transpilation complete");
		if(ThereWasAnError()){
			printf(" with errors");
		}
		printf("!\r\n");		

		FreeProgram(prog);
		free(ventSrc);
}

int main(int argc, char* argv[]) {

	if(argc < 2){
		PrintUsage();
		exit(EXIT_FAILURE);
	}	

	bool printProgramTree = false;
	bool printTokens = false;

	if(argc == 3){
		if(strcmp("--print-tokens", argv[2]) == 0){
			printTokens = true;
		} else if(strcmp("--print-ast", argv[2]) == 0){
			printProgramTree = true;
		}
	}
	
	doTranspile(argv[1], printProgramTree, printTokens);

	return 0;
}

