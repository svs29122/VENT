#ifndef INC_DBA_H
#define INC_DBA_H
/*
	Dynamic array akin to a C++ vector
	
	When to use:
		use when you need a contiguous array of blocks in memory all 
		of the same size. The array automatically resizes during
		writes if there is not enough space in the array. Array must
		be manually freed when no longer needed.

		for dynamically sized arrays with dynamically sized elements
		use a list instead
*/

//TODO: add some sort of iterator interface to this structure

typedef struct DynamicBlockArray {
	int count; 
	int capacity; 
	size_t blockSize;
	char* block;
} Dba;

/************************
	InitBlockArray() - creates a dynamic array of elements of size bsize on the heap
		and returns a pointer to that memory.  

	Inputs: 
		bsize - size of elements to be stored in array

	Outputs:

	Returns:
		pointer to the new heap allocated array or NULL of allocation failed 

*/
Dba* InitBlockArray(size_t bsize);

/************************
	FreeBlockArray() - frees the dynamic array allocated earlier and sets handle to NULL  

	Inputs: 
		arr - pointer to a dynamic block array

	Outputs:

	Returns:

*/
void FreeBlockArray(Dba* arr);

/************************
	WriteBlockArray() - writes a block of data to the dynamic array, will realloc
		if at capacity  

	Inputs: 
		arr - pointer to a dynamic block array 
		block - pointer to data to be written to array, must be of size
			blockSize

	Outputs:

	Returns:

*/
void WriteBlockArray(Dba* arr, char* block);

#endif 
