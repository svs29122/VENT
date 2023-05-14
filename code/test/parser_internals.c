#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lexer.h>
#include <ast.h>

#include "cutest.h"

void TestParseInternal_SimpleProcess(CuTest *tc){
   char* input = strdup(" \
      proc ( clk ) {\n \
			clk <=  not clk;\n \
		}\n \
   ");

   free(input);
}

void TestParseInternal_(CuTest *tc){
   char* input = strdup(" \
      \
   ");

   free(input);
}

CuSuite* ParserTestInternalsGetSuite(){
   CuSuite* suite = CuSuiteNew();

   SUITE_ADD_TEST(suite, TestParseInternal_SimpleProcess);
   SUITE_ADD_TEST(suite, TestParseInternal_);

   return suite;
}

