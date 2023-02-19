#include <stdio.h>
#include <unistd.h>

void printUsage(void){
	printf("Usage:\n"
			" tvt -i (interactive menu)\n"
			" tvt -t adder.vent (perform transpilation)\n"
			" tvt -r adder.vhdl (perform reverse transpilation\n"
		);
}

void printOptions(void){
	printf(" t:   \tTranspile VENT to VHDL\n"
		 	 " r:   \tReverse Transpile VHDL to VENT\n"
			 " h:   \tHelp\n"
			 " q/Q: \tQuit\n"); 
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

