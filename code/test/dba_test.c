#include <stdio.h>
#include <time.h>

#include "dba.h"

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

void testBlockUnion() {
	
	Dba myBlock;
	size_t mySize = sizeof(union testUnion);
	initBlockArray(&myBlock, mySize);

	union testUnion b1;
	b1.m1.a = 1;
	b1.m1.b = 2;
	b1.m1.c = 3;

	writeBlockArray(&myBlock, (char*)&b1);
	
	b1.m2.a = 'a';
	b1.m2.b = 'b';
	b1.m2.c = 'c';
	
	writeBlockArray(&myBlock, (char*)&b1);

	b1.m3.a = 1.0;
	b1.m3.b = 2.0;
	b1.m3.c = 3.0;

	writeBlockArray(&myBlock, (char*)&b1);

	printf("Block Array Status: %d, %d, %ld, %p\r\n\n", 
		myBlock.count, myBlock.capacity, myBlock.blockSize, myBlock.block);

	//print all data
	for(int i=0; i<myBlock.count; i++){
		printf("Block %d:\r\n ", i+1);
			union testUnion* tb = (union testUnion*) (myBlock.block + (i*myBlock.blockSize));
			if(i == 0)
				printf("%d, %d, %d\r\n", tb->m1.a, tb->m1.b, tb->m1.c);  
			if(i == 1)
				printf("%c, %c, %c\r\n", tb->m2.a, tb->m2.b, tb->m2.c);  
			if(i == 2)
				printf("%f, %f, %f\r\n", tb->m3.a, tb->m3.b, tb->m3.c);  
	}

	freeBlockArray(&myBlock);
}

struct testBlock {
	int a;
	char b;
	float c;
};

void testBlockStruct(){
	
	Dba myBlock;
	size_t mySize = sizeof(struct testBlock);
	initBlockArray(&myBlock, mySize);

	struct testBlock b1;
	b1.a = 10;
	b1.b = 'A';
	b1.c = 5.0;

	struct testBlock b2;
	b2.a = 24;
	b2.b = 'Q';
	b2.c = 2.32;	

	struct testBlock b3;
	b3.a = 99;
	b3.b = 'Y';
	b3.c = 0.0002;

	struct testBlock b4;
	b4.a = -78;
	b4.b = '@';
	b4.c = 3.0128e28;

	struct testBlock b5;
	b5.a = 13;
	b5.b = 'x';
	b5.c = 1.01;

	struct testBlock b6;
	b6.a = 902;
	b6.b = '{';
	b6.c = 2e28;

	struct testBlock b7;
	b7.a = 100;
	b7.b = '?';
	b7.c = 0.08e2;

	struct testBlock b8;
	b8.a = -909;
	b8.b = 'a';
	b8.c = 3.14;

	struct testBlock b9;
	b9.a = 101010;
	b9.b = '#';
	b9.c = 18e28;

	struct testBlock b10;
	b10.a = 8;
	b10.b = '!';
	b10.c = 1.0;

	struct testBlock b11;
	b11.a = 23;
	b11.b = 'z';
	b11.c = 38;

	writeBlockArray(&myBlock, (char*)&b1);
	writeBlockArray(&myBlock, (char*)&b2);
	writeBlockArray(&myBlock, (char*)&b3);
	writeBlockArray(&myBlock, (char*)&b4);
	writeBlockArray(&myBlock, (char*)&b5);
	writeBlockArray(&myBlock, (char*)&b6);
	writeBlockArray(&myBlock, (char*)&b7);
	writeBlockArray(&myBlock, (char*)&b8);
	writeBlockArray(&myBlock, (char*)&b9);
	writeBlockArray(&myBlock, (char*)&b10);
	writeBlockArray(&myBlock, (char*)&b11);

	printf("Block Array Status: %d, %d, %ld, %p\r\n\n", 
		myBlock.count, myBlock.capacity, myBlock.blockSize, myBlock.block);

	//print all data
	for(int i=0; i<myBlock.count; i++){
		printf("Block %d:\r\n ", i+1);
			struct testBlock* tb = (struct testBlock*) (myBlock.block + (i*myBlock.blockSize));
			printf("%d, %c, %f\r\n", tb->a, tb->b, tb->c);  
	}

	freeBlockArray(&myBlock);
}

void testBlockArray(){

	Dba myBlock;
	size_t mySize = 28;
	initBlockArray(&myBlock, mySize);

	char testData[28];
	for(int i=0; i<28; i++){
		testData[i] = i;
	}
	writeBlockArray(&myBlock, testData);
	for(int i=0; i<28; i++){
		testData[i] = i*2;
	}
	writeBlockArray(&myBlock, testData);
	for(int i=0; i<28; i++){
		testData[i] = i*3;
	}
	writeBlockArray(&myBlock, testData);

	printf("Block Array Status: %d, %d, %ld, %p\r\n\n", 
		myBlock.count, myBlock.capacity, myBlock.blockSize, myBlock.block);

	//print all data
	for(int i=0; i<myBlock.count; i++){
		printf("Block %d:\r\n [", i+1);
		for(int j=0; j<myBlock.blockSize; j++){
			int index = i*myBlock.blockSize + j;
			printf("%d,", myBlock.block[index]);
		}
		printf("]\r\n");
	}
	
	freeBlockArray(&myBlock);
}

int main(){

	printf("\e[1;37mTest of the Dynamic Block Array\r\n\n\e[0m");

	printf("\e[4;37mTest 1: Using an array\r\n\e[0m");
	float timeA = (float)clock()/CLOCKS_PER_SEC;
	testBlockArray();
	float timeB = (float)clock()/CLOCKS_PER_SEC;
	printf("\n\e[1;32mTest1 took %f\r\n\e[0m", timeB-timeA);

	printf("\e[4;37m\n\nTest 2: Using a struct\r\n\e[0m");
	timeA = (float)clock()/CLOCKS_PER_SEC;
	testBlockStruct();
	timeB = (float)clock()/CLOCKS_PER_SEC;
	printf("\n\e[1;32mTest2 took %f\r\n\e[0m", timeB-timeA);

	printf("\e[4;37m\n\nTest 3: Using a union\r\n\e[0m");
	timeA = (float)clock()/CLOCKS_PER_SEC;
	testBlockUnion();
	timeB = (float)clock()/CLOCKS_PER_SEC;
	printf("\n\e[1;32mTest3 took %f\r\n\e[0m", timeB-timeA);

	printf("\nDone...\n\n");

	return 0;
}
