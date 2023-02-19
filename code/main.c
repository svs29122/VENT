#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lexer.h"
#include "display.h"

void doTranspile(char* fileName);
void doRevTranspile(char* fileName);

int main(int argc, char* argv[]) {

	if(argc < 2){
		printUsage();
		return -1;
	}	

	if(strcmp("-t", argv[1]) == 0){
		doTranspile(argv[2]);
	} else if(strcmp("-i", argv[1]) == 0){
		 doMenu();
	}

	return 0;
}

void doTranspile(char* fileName){
		FILE* ventFile = fopen(fileName, "r");
	
		if(!ventFile){
			perror("Unable to open file!\n");
		}
		
		struct lexer *myLexer = initLexer(ventFile);
	
		while(EOF != readChar(myLexer)){
			printf("%c", getChar(myLexer));	
		}
		
		free(myLexer);
}

void doRevTranspile(char* fileName){
}

