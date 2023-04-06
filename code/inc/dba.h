#ifndef INC_DBA_H
#define INC_DBA_H

typedef struct DynamicBlockArray {
	int count; 
	int capacity; 
	size_t blockSize;
	char* block;
} Dba;

Dba* InitBlockArray(size_t bsize);
void FreeBlockArray(Dba* arr);
void WriteBlockArray(Dba* arr, char* block);

#endif 
