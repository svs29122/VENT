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

struct MessageBuffer {
	char* buffer;
	int msgCount;
	const int MAXBUFF;
	const int MAXMSG;
};

struct MessageBuffer checkSyntaxErrorsInGeneratedVHDL(){
	//TODO: find a better way to check VHDL syntax, this takes forever
	char* cmd = " /tools/Xilinx/Vivado/2022.1/bin/vivado -mode batch -nolog -nojournal \
					-quiet -source ./test/check_vent_output.tcl -notrace -tclargs a.vhdl";

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

	//checkForSyntaxErrors(tc);

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

	//checkForSyntaxErrors(tc);
	
	FreeProgram(prog);
	free(input);
	remove("./a.vhdl");
}

CuSuite* TranspileTestGetSuite(){

	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestTranspileProgram_Simple);
	SUITE_ADD_TEST(suite, TestTranspileProgram_WithProcess);

	return suite;
}

