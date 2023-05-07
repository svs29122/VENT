#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dba.h>

struct DynamicBlockArray {
	int count;
   int capacity;
   size_t blockSize;
   char* block;
};

struct DynamicBlockArray* InitBlockArray(size_t bsize){	
	Dba* arr = malloc(sizeof(Dba));	
	if(arr == NULL){
		printf("Error: Unable to allocate Block Array\r\n");
		exit(-1);
	}

	arr->count = 0;
	arr->capacity = 0;
	arr->blockSize = bsize;
	arr->block = NULL;
	
	return arr;
} 

void FreeBlockArray(struct DynamicBlockArray* arr){
	arr->count = 0;
	arr->capacity = 0;
	arr->blockSize = 0;

	free(arr->block);
	arr->block = NULL;
	
	free(arr);
}

void WriteBlockArray(struct DynamicBlockArray* arr, char* block){
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

void* ReadBlockArray(struct DynamicBlockArray* arr, int index){
	if(arr == NULL){
		printf("Error: Block Array Ptr NULL\r\n");
		return NULL;
	}	

	if(index >= arr->count) {
		printf("Error: Block Array index out of bounds\r\n");
		return NULL;
	}
	
	return (arr->block + (index * arr->blockSize)); 
}

int BlockCount(struct DynamicBlockArray* arr){
	if(arr == NULL){
		printf("Error: Block Array Ptr NULL\r\n");
		return 0;
	}	

	return arr->count;
}
