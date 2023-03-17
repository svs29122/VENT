#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dba.h"

void initBlockArray(Dba* arr, size_t bsize){	
	arr->count = 0;
	arr->capacity = 0;
	arr->blockSize = bsize;
	arr->block = NULL;
} 

void freeBlockArray(Dba* arr){
	free(arr->block);
	initBlockArray(arr, 0);
}

void writeBlockArray(Dba* arr, char* block){
	if(arr == NULL) {
		printf("Error: Block Array Ptr NULL\r\n");
		return;
	} 

	if(arr->capacity < arr->count + 1){
		int oldCapacity = arr->capacity;
		arr->capacity = oldCapacity < 2 ? 2 : (oldCapacity * 2);
		arr->block = (char*) realloc(arr->block, (arr->blockSize * arr->capacity)); 
	}

	char* blockPtr = &arr->block[arr->count * arr->blockSize];
	memcpy(blockPtr, block, arr->blockSize);
	arr->count++;
}
