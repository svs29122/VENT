#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "inc/lexer.h"

void printWaterfall(void);
void printOptions(void);
void doMenu(void);

int main(int argc, char* argv[]) {

	if(argc < 2){
		printf("Usage:\n"
				" tvt -m (interactive menu)\n"
				" tvt -t adder.vent (perform transpilation)\n"
				" tvt -r adder.vhdl (perform reverse transpilation)\n"         
				"");
		return -1;
	}	

	if(strcmp("-m", argv[1]) == 0){
		 doMenu();
	} else if(strcmp("-t", argv[1]) == 0){
		FILE* vhdFile = fopen(argv[2], "r");
	
		if(!vhdFile){
			perror("Unable to open file!\n");
			return -1;
		}
		
		struct lexer *myLexer = initLexer(vhdFile);
	
		while(EOF != readChar(myLexer)){
			printf("%c", getChar(myLexer));	
		}
		
		free(myLexer);
	}

	return 0;
}


void printWaterfall(void){
	
	const int waterFallDelay = 250000;

	printf("-----------------------\n");
	printf("The V E N T Transpiler\n");
	usleep(waterFallDelay);
	printf("     H x o e\n");
	usleep(waterFallDelay);
	printf("      D c t r\n");
	usleep(waterFallDelay);
	printf("       L e   r\n");
	usleep(waterFallDelay);
	printf("          p   i\n");
	usleep(waterFallDelay);
	printf("           t   b\n");
	usleep(waterFallDelay);
	printf("                l\n");
	usleep(waterFallDelay);
	printf("                 e\n");
	usleep(waterFallDelay);
	printf("-----------------------\n");
}

void printOptions(void){

	printf(" t:   \tTranspile VENT to VHDL\n"
		 	 " r:   \tReverse Transpile VHDL to VENT\n"
			 " h:   \tHelp\n"
			 " q/Q: \tQuit\n"); 

}

void doMenu(void){

	printWaterfall();

	char opt[256];
	while(1){
		printf("What would you like to do?\n");
		printOptions();
		
		printf("\n=>");
		fgets(opt, sizeof(opt), stdin);
		
		switch(opt[0]){
		   
			case 't':
				printf("Transpiling...\n");
				sleep(3);
				break;
		
			case 'r':
				printf("Reverse transpiling...\n");
				sleep(3);
				break;

			case 'h':
				printf("Help:\n");
				break;
			
			case 'Q':
			case 'q':
				goto exit;
		
			default:
				break;
		}
	}

	exit:
		return;
}
