#include <stdio.h>

#include "inc/lexer.h"

int main(int argc, char* argv[]) {

	printf("Hello, welcome to The VENT Transpiler!\n");

	if(argc < 2){
		printf("Usage: tvt adder.vhd\n");
		return -1;
	}	

	FILE* vhdFile = fopen(argv[1], "r");
	
	if(!vhdFile){
		perror("Unable to open file!\n");
		return -1;
	}

	struct lexer *myLexer = initLexer(vhdFile);
	
	while(EOF != readChar(myLexer)){
		printf("%c", getChar(myLexer));	
	}


	return 0;
}
