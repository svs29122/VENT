#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"
#include "display.h"

static void doTranspile(char* fileName);
static void doRevTranspile(char* fileName);

int main(int argc, char* argv[]) {

	if(argc < 2){
		PrintUsage();
		return -1;
	}	

	if(argc == 2){
		doTranspile(argv[1]);
	} else if(strcmp("-i", argv[1]) == 0){
		 DoMenu();
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

static void doTranspile(char* fileName){
		char* ventSrc = readFile(fileName);
		InitLexer(ventSrc);
		
		Token t = NextToken();
		while(t.type != ILLEGAL && t.type != EOP){
			PrintToken(t);
			free(t.literal);

			t = NextToken();
		}
		
		if(t.type == ILLEGAL) PrintToken(t);
		free(t.literal);
		
		free(ventSrc);
}

static void doRevTranspile(char* fileName){
}

