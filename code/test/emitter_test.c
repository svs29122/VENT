#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <lexer.h>
#include <parser.h>
#include <display.h>
#include <emitter.h>

#include "cutest.h"

//#define CHECK_VHDL_SYNTAX 

struct MessageBuffer {
	char* buffer;
	int msgCount;
	const int MAXBUFF;
	const int MAXMSG;
};

struct MessageBuffer checkSyntaxErrorsInGeneratedVHDL(){
	//TODO: find a better way to check VHDL syntax, this takes forever
	char* cmd = " /tools/Xilinx/Vivado/2022.1/bin/vivado -mode batch -nolog -nojournal \
					-quiet -source ./test/check_vent.tcl -notrace -tclargs a.vhdl";

	struct MessageBuffer mBuff = {NULL, 0, 8192, 32};
	mBuff.buffer =  calloc(1, sizeof(char) * mBuff.MAXBUFF);
	char* buffPtr = mBuff.buffer;

	//start with a newline
	*buffPtr++ = '\n';
	
	//run the Vivado syntax checker
	FILE* vOut = popen(cmd, "r");

	size_t maxSize = 256;
	int msgSize = 0;
	char* tmpBuf = NULL ;

	for(int i=0; i < mBuff.MAXMSG; i++){
		msgSize = getline(&tmpBuf, &maxSize, vOut);
	
		if(msgSize != -1 && msgSize > 1) {
			strncpy(buffPtr, tmpBuf, msgSize);

			//concating messages in the buffer
			buffPtr[msgSize-1] = ' ';
			buffPtr[msgSize] = '\n';

			buffPtr += msgSize + 1;
			mBuff.msgCount++;
		} else break;
	}

	pclose(vOut);

	return mBuff;
}

void checkForSyntaxErrors(CuTest* tc){
	struct MessageBuffer errorMessages = checkSyntaxErrorsInGeneratedVHDL();
	bool syntaxErrorFree = errorMessages.msgCount == 0;

	if(!syntaxErrorFree){
		const int maxNameSize = 512;
		char copyCommand[maxNameSize];
		bool success = (-1) != (snprintf(copyCommand, maxNameSize,
			"cp ./a.vhdl ./FAILED_%s.vhdl", tc->name));
		
		if(success) {
			system(copyCommand);
			remove("./a.vhdl");
		}
		printf("*%s completed but had syntax errors.\r\n", tc->name);
	} else {
		printf("%s completed without syntax errors.\r\n", tc->name);
	}

	CuAssert(tc, errorMessages.buffer, syntaxErrorFree);
	free(errorMessages.buffer);
}

void TestTranspileProgram_Simple(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent ander { \
			a -> stl; \
			b -> stl; \
			y <- stl; \
		} \
		\
		arch behavioral(ander){ \
			sig temp stl := '0'; \
		\
			temp <= a and b; \
			y <= temp; \
			} \
		");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithProcess(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent ander { \
			a -> stl; \
			b -> stl; \
			y <- stl; \
		} \
		\
		arch behavioral(ander){ \
			sig temp stl := '0'; \
		\
			proc(){ \
				var i int := 1; \
				while( i < 10 ) { \
					i := i + 1; \
				} \
				wait; \
			} \
		} \
		");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif
	
	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithLoops(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent looper { \
			a -> stl; \
			b -> stl; \
			y <- stl; \
		} \
		\
		arch myArch(looper) { \
			sig s stl; \
			type opCode {Idle, Start, Stop, Clear, 'Q'}; \
			\
   		proc () { \
      		var count int := 0;     \
				 \
      		loop { \
         		count := count + 1; \
         		count += 1; \
         		count++;  \
         		count := count - 1; \
         		count -= 1; \
         		count--;  \
         		count *= 2; \
         		count /= 4; \
      		} \
				wait; \
   		} \
		 \
   		proc () { \
      		var i int; \
   			\
      		while(i < 10){ \
         		s <= '1'; \
         		i := i + 2; \
      		}    \
      		wait; \
   		}    \
		  \
   		proc () { \
      		for (i : 0 to 5) { \
         		assert (i != 10) report \"i out of bounds\" severity error; \
      		} \
		 		\
      		for (op : Opcode) { \
         		report \"op\" severity note; \
      		} \
				wait; \
   		} \
		} \
			 \
		");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif
	
	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithIfs(CuTest* tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent ifer { \
			a <- stl; \
		} \
		\
      arch myArch(ifer){\n \
         \n \
         proc () {\n \
				var i int := 1;\n \
				\n \
            if(i<1){\n \
               a <= '0';\n \
            } elsif (i>2){\n \
               a <= '1';\n \
            } else {\n \
               a <= 'Z';\n \
            }\n \
				\n \
				wait;\n \
         }\n \
      }\n \
   ");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);
#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif
	
	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithMultipleIfs(CuTest* tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent ifer { \
			a <- stl; \
		} \
		\
      arch myArch(ifer){\n \
         \n \
         proc () {\n \
				var i int := 1;\n \
				\n \
            if(i<1){\n \
               a <= '0';\n \
            }\n \
				\n \
				if(i==2){\n \
					a <= 'X';\n \
				} elsif (i>2){\n \
               a <= '1';\n \
            }\n \
				\n \
				if(i==7){\n \
					a <= '0';\n \
				} else {\n \
               a <= 'Z';\n \
            }\n \
				\n \
				wait;\n \
         }\n \
      }\n \
   ");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);
#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif
	
	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithNestedIfs(CuTest* tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent ifer { \
			a <- stl; \
			b <- stl; \
			c <- stl; \
		} \
		\
      arch myArch(ifer){\n \
         \n \
         proc () {\n \
				var i int := 1;\n \
				var j int := 2;\n \
				var k int := 3;\n \
				\n \
            if(i<j){\n \
               a <= '1';\n \
            } elsif (j<i){\n \
               b <= '1';\n \
               if(k == 1){\n \
                  b <= '0';\n \
               } elsif (k == 0) {\n \
                  b <= '1';\n \
               } else {\n \
                  b <= '0';\n \
               }\n \
            } else {\n \
               a <= '0';\n \
               b <= '0';\n \
            }\n \
				\n \
				wait;\n \
         }\n \
      }\n \
   ");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);
#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif
	
	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithSwitchCase(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all;\n \
		ent switcher {\n \
			A <- stl;\n \
			B <- stl;\n \
			C <- stl;\n \
			D <- stl;\n \
		}\n \
      arch behavioral(switcher){\n \
         \n \
         proc() {\n \
				var ADDRESS int := 2;\n \
				\n \
            switch(ADDRESS) {\n \
               case 0:\n \
                  A <= '1';\n \
               case 1:\n \
                  A <= '1';\n \
                  B <= '1';\n \
               case 2 to 15:\n \
                  C <= '1';\n \
               case 16 | 10 downto 20 | 25:\n \
                  B <= '1';\n \
                  C <= '1';\n \
                  D <= '1';\n \
               default:\n \
                  null;\n \
            }\n \
				wait;\n \
         }\n \
      }\n \
      \
   ");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);
#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithGenerics(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all; \
		\
		ent ander { \
			g int := 1;\
			h int := 2;\
			a -> stl; \
			b -> stl; \
			k int := 3;\
			y <- stl; \
		} \
		\
		arch behavioral(ander){ \
			sig temp stl := '0'; \
		\
			temp <= a and b; \
			y <= temp; \
			} \
		");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithComponent(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all;\n \
		\n\
		ent ander {\n \
			a -> stl;\n \
			b -> stl;\n \
			y <- stl;\n \
		}\n \
		\n\
		arch behavioral(ander){\n \
		\n \
      	comp counter {\n \
            SIZE int := 256;\n \
            LENGTH int;\n \
            clk -> stl;\n \
            rst -> stl;\n \
            upDown -> stl;\n \
            Q <- stlv(3 downto 0);\n \
         }\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_WithInstantiation(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all;\n \
		ent counter {\n \
		}\n \
		arch behavioral(counter){\n \
			\n \
			//declare the counter\n \
			comp counter {\n \
				SIZE int := 64;\n \
				ADDR int := 64;\n \
				clk -> stl;\n \
				rst -> stl;\n \
				Q <- stlv(3 downto 0);\
			}\n \
			\n \
			//declare the signals\n \
			sig clk stl;\n \
			sig rst stl;\n \
			sig Q stlv(3 downto 0);\n \
			\n \
			//instanitate 3 counters\n \
			C1: counter map(8, 16, clk, rst, Q);\n \
      	\n \
      	C2: counter map (\n \
				SIZE => 32,\n \
				ADDR => 32,\n \
         	clk => clk,\n \
         	rst => rst,\n \
         	Q => open,\n \
         	);\n \
         	\n \
			C3: counter map (*);\n \
			C4: counter map (128, 123456789, *);\n \
			C5: counter map (128, *);\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);
	//PrintProgram(prog);

	TranspileProgram(prog, NULL);

#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

void TestTranspileProgram_(CuTest *tc){
	char* input = strdup(" \
		use ieee.std_logic_1164.all;\n \
		ent space {\n \
		}\n \
		arch empty(space){\n \
		}\n \
	");

	struct Program* prog = ParseProgram(input);

	TranspileProgram(prog, NULL);

#ifdef CHECK_VHDL_SYNTAX
	checkForSyntaxErrors(tc);
#endif

	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

CuSuite* TranspileTestGetSuite(){

	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestTranspileProgram_Simple);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithProcess);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithLoops);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithIfs);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithMultipleIfs);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithNestedIfs);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithSwitchCase);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithGenerics);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithComponent);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithInstantiation);

	return suite;
}

