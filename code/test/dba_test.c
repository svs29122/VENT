#include <stdio.h>
#include <time.h>

#include "cutest.h"
#include "dba.h"

void TestDba_SimpleBlockArray(CuTest* tc){

	#define DATA_SIZE 128
	struct testBlock{
		int data[DATA_SIZE];
	};

	Dba* myBlock = InitBlockArray(sizeof(struct testBlock));

	struct testBlock tmp;
   for(int i=0; i<DATA_SIZE; i++){
      tmp.data[i] = i;
   }  
	WriteBlockArray(myBlock,(char*)&tmp);
   for(int i=0; i<DATA_SIZE; i++){
      tmp.data[i] = i*2;
   }  
	WriteBlockArray(myBlock,(char*)&tmp);
   for(int i=0; i<DATA_SIZE; i++){
      tmp.data[i] = i*3;
   }  
	WriteBlockArray(myBlock,(char*)&tmp);

	for(int i=0; i<BlockCount(myBlock); i++){
		struct testBlock* storedBlock = (struct testBlock*)ReadBlockArray(myBlock, i);
		for(int j=0; j<DATA_SIZE; j++){
			CuAssertIntEquals(tc, j*(i+1), storedBlock->data[j]);
		}
	}
	#undef DATA_SIZE

	FreeBlockArray(myBlock);
}

void TestDba_ArrayOfStructs(CuTest* tc){
	
	struct testBlock {
		int a;
		char b;
		float c;
	};

	Dba* myBlock = InitBlockArray(sizeof(struct testBlock));

	struct testBlock b[12];
	b[0].a = 54;
	b[0].b = 'l';
	b[0].c = 213.10;

	b[1].a = 10;
	b[1].b = 'A';
	b[1].c = 5.0;

	b[2].a = 24;
	b[2].b = 'Q';
	b[2].c = 2.32;	

	b[3].a = 99;
	b[3].b = 'Y';
	b[3].c = 0.0002;

	b[4].a = -78;
	b[4].b = '@';
	b[4].c = 3.0128e28;

	b[5].a = 13;
	b[5].b = 'x';
	b[5].c = 1.01;

	b[6].a = 902;
	b[6].b = '{';
	b[6].c = 2e28;

	b[7].a = 100;
	b[7].b = '?';
	b[7].c = 0.08e2;

	b[8].a = -909;
	b[8].b = 'a';
	b[8].c = 3.14;

	b[9].a = 101010;
	b[9].b = '#';
	b[9].c = 18e28;

	b[10].a = 8;
	b[10].b = '!';
	b[10].c = 1.0;

	b[11].a = 23;
	b[11].b = 'z';
	b[11].c = 38;

	WriteBlockArray(myBlock, (char*)&b[0]);
	WriteBlockArray(myBlock, (char*)&b[1]);
	WriteBlockArray(myBlock, (char*)&b[2]);
	WriteBlockArray(myBlock, (char*)&b[3]);
	WriteBlockArray(myBlock, (char*)&b[4]);
	WriteBlockArray(myBlock, (char*)&b[5]);
	WriteBlockArray(myBlock, (char*)&b[6]);
	WriteBlockArray(myBlock, (char*)&b[7]);
	WriteBlockArray(myBlock, (char*)&b[8]);
	WriteBlockArray(myBlock, (char*)&b[9]);
	WriteBlockArray(myBlock, (char*)&b[10]);
	WriteBlockArray(myBlock, (char*)&b[11]);

	for(int i=0; i<BlockCount(myBlock); i++){
			struct testBlock* tb = (struct testBlock*) ReadBlockArray(myBlock, i);
			CuAssertIntEquals(tc, b[i].a, tb->a);
			CuAssertIntEquals(tc, b[i].b, tb->b);
			CuAssertDblEquals(tc, b[i].c, tb->c, 0.0000001);
	}

	FreeBlockArray(myBlock);
}

void TestDba_ArrayOfUnions(CuTest* tc) {
	
	union testUnion{
		struct member1 {
			int a;
			int b;
			int c;
		} m1;	
	
		struct member2 {
			char a;
			char b;
			char c;
		} m2;
	
		struct member3 {
			double a;
			double b;
			double c;
		} m3;
	};

	Dba* myBlock = InitBlockArray(sizeof(union testUnion));

	union testUnion b1;
	b1.m1.a = 1;
	b1.m1.b = 2;
	b1.m1.c = 3;

	WriteBlockArray(myBlock, (char*)&b1);
	
	b1.m2.a = 'a';
	b1.m2.b = 'b';
	b1.m2.c = 'c';
	
	WriteBlockArray(myBlock, (char*)&b1);

	b1.m3.a = 1.0;
	b1.m3.b = 2.0;
	b1.m3.c = 3.0;

	WriteBlockArray(myBlock, (char*)&b1);

	//print all data
	for(int i=0; i<BlockCount(myBlock); i++){
			union testUnion* tb = (union testUnion*) ReadBlockArray(myBlock, i);
			if(i == 0){
				CuAssertIntEquals(tc, 1, tb->m1.a);
				CuAssertIntEquals(tc, 2, tb->m1.b);
				CuAssertIntEquals(tc, 3, tb->m1.c);
			} else if(i == 1) {
				CuAssertIntEquals(tc, 'a', tb->m2.a);
				CuAssertIntEquals(tc, 'b', tb->m2.b);
				CuAssertIntEquals(tc, 'c', tb->m2.c);
			} else if(i == 2) {
				CuAssertDblEquals(tc, 1.0, tb->m3.a, 0.0000001);
				CuAssertDblEquals(tc, 2.0, tb->m3.b, 0.0000001);
				CuAssertDblEquals(tc, 3.0, tb->m3.c, 0.0000001);
			}
	}

	FreeBlockArray(myBlock);
}

CuSuite* DbaTestGetSuite(){
	CuSuite* suite = CuSuiteNew();

	SUITE_ADD_TEST(suite, TestDba_SimpleBlockArray);
	SUITE_ADD_TEST(suite, TestDba_ArrayOfStructs);
	SUITE_ADD_TEST(suite, TestDba_ArrayOfUnions);

	return suite;
}
