#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dba.h"

Dba* InitBlockArray(size_t bsize){	
	Dba* arr = malloc(sizeof(Dba));	
	if(arr == NULL){
		printf("Error: Unable to allocated Block Array\r\n");
		exit(-1);
	}

	arr->count = 0;
	arr->capacity = 0;
	arr->blockSize = bsize;
	arr->block = NULL;
	
	return arr;
} 

void FreeBlockArray(Dba* arr){
	arr->count = 0;
	arr->capacity = 0;
	arr->blockSize = 0;

	free(arr->block);
	arr->block = NULL;
	
	free(arr);
}

void WriteBlockArray(Dba* arr, char* block){
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
