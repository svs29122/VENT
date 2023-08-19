#ifndef INC_DBA_H
#define INC_DBA_H

#include <stddef.h>

/*
	Dynamic array akin to a C++ vector
	
	When to use:
		use when you need a dynamic, contiguous array of blocks in memory 
		all of the same size. The array automatically resizes during
		writes if there is not enough space in the array. Array must
		be manually freed when no longer needed.

		for dynamic arrays with elements of different or variable 
		size use a list instead
*/

typedef struct DynamicBlockArray Dba;

/************************
	InitBlockArray() - creates a dynamic array of elements of size bsize on the heap
		and returns a pointer to that memory.  

	Inputs: 
		bsize - size of elements to be stored in array

	Outputs:

	Returns:
		pointer to the new heap allocated array or NULL if allocation failed 

*/
Dba* InitBlockArray(size_t bsize);

/************************
	FreeBlockArray() - frees the dynamic array allocated earlier and sets pointer to NULL  

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
		block - pointer to data to be written to array, must be of size blockSize

	Outputs:

	Returns:

*/
void WriteBlockArray(Dba* arr, char* block);

/************************
	ReadBlockArray() - returns a block from the array located
		at index (0-based indexing)

	Inputs: 
		arr - pointer to a dynamic block array 
		index - block to be accessed within array

	Outputs:

	Returns:
		void* to block element (must be cast to correct object type)
		NULL when arr == NULL or index >= count

*/
void* ReadBlockArray(Dba* arr, int index);

/************************
	BlockCount() - returns the current number of blocks in the array

	Inputs: 
		arr - pointer to a dynamic block array 

	Outputs:

	Returns:
		int equal to arr->count
		NULL when arr == NULL

*/
int BlockCount(Dba* arr);


#endif //INC_DBA_H 
