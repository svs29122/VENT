#ifndef INC_DBA_H
#define INC_DBA_H

typedef struct DynamicBlockArray {
	int count; 
	int capacity; 
	size_t blockSize;
	char* block;
} Dba;

void initBlockArray(Dba* arr, size_t bsize);
void freeBlockArray(Dba* arr);
void writeBlockArray(Dba* arr, char* block);

#endif 
