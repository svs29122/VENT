#include <stdlib.h>
#include <string.h>

#include "dba.h"

void* reallocate(void* pointer, size_t oldSize, size_t newSize){
	if(newSize == 0){
		free(pointer);
		return NULL;
	}

	void* result = realloc(pointer, newSize);
	if(result == NULL) exit(1);
	return result;
}

void initBlockArray(Dba* arr, size_t bsize){	
	arr->count = 0;
	arr->capacity = 0;
	arr->blockSize = bsize;
	arr->data = NULL;
} 

void freeBlockArray(Dba* arr){
	FREE_ARRAY(arr->blockSize, arr->data, arr->capacity);
	initBlockArray(arr, 0);
}

void writeBlockArray(Dba* arr, char* block){
	if(arr->capacity < arr->count + 1){
		int oldCapacity = arr->capacity;
		arr->capacity = GROW_CAPACITY(oldCapacity);
		arr->data = GROW_ARRAY(arr->blockSize, arr->data, oldCapacity, arr->capacity);
	}

	char* dataPtr = &arr->data[arr->count * arr->blockSize];
	memcpy(dataPtr, block, arr->blockSize);
	arr->count++;
}
