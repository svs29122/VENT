#ifndef INC_DBA_H
#define INC_DBA_H

#define GROW_CAPACITY(capacity) \
	((capacity) < 8 ? 8 : (capacity * 2))

#define GROW_ARRAY(blockSize, pointer, oldCount, newCount) \
	(char*)reallocate(pointer, blockSize * (oldCount), \
		blockSize * (newCount))

#define FREE_ARRAY(blockSize, pointer, oldCount) \
	reallocate(pointer, blockSize * (oldCount), 0)

void* reallocate(void* pointer, size_t oldSize, size_t newSize);

// DBA implementation 
typedef struct DynamicBlockArray {
	int count; //num of blocks currently in array
	int capacity; //num of blocks that can fit in array
	size_t blockSize;
	char* data;
} Dba;

void initBlockArray(Dba* arr, size_t bsize);
void freeBlockArray(Dba* arr);
void writeBlockArray(Dba* arr, char* block);

#endif 
