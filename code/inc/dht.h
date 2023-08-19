#ifndef INC_DHT_H
#define INC_DHT_H

#include <stdbool.h>
#include <stdint.h>

/*
	Dynamic hash table (or map)

	When to use:
		use when you need a dynamic map that allows fast lookup via strings.
		All keys should be strings and all values should be uint64_ts (this 
		so we can store pointers if necessary). The hash table automatically
		resizes based on a preset load factor and uses linear probing instead
		of linked lists for collisions. The Hash function is FNV-1a

		there is also an iterator (HashTableIterator) for walking every entry
		in the table, although in no particular order i.e. entries are unsorted. 

*/

/************************
   InitHashTable() - creates a dynamic hashTable on the heap
      and returns a pointer to that memory. Initial size is 8 elements. 

   Inputs: 

   Outputs:

   Returns:
      pointer to the new heap allocated table or NULL if allocation failed 

*/
struct DynamicHashTable* InitHashTable();

/************************
   FreeHashTable() - frees the dynamic hash table allocated earlier and sets pointer to NULL  

   Inputs: 
      hst - pointer to a dynamic hash table

   Outputs:

   Returns:

*/
void FreeHashTable(struct DynamicHashTable* hst);

/************************
   SetInHashTable() - adds entry to  dynamic hash table 

   Inputs: 
      hst - pointer to a dynamic hash table
      key - string representing lookup value
		val - uint64_t to store in table at index computed by key

   Outputs:

   Returns:
		true if new key/value pair was added to table
		false if hash table pointer null or key already exists in table

*/
bool SetInHashTable(struct DynamicHashTable* hst, char* key, uint64_t val);

/************************
   GetInHashTable() - checks dynamic hash table for entry

   Inputs: 
      hst - pointer to a dynamic hash table
      key - string representing entry in table

   Outputs:
		val - pointer to output paramter holding value corersponding to key

   Returns:
		true if key was located in table
		false if hash table pointer null, table has zero entries, or key not found

*/
bool GetInHashTable(struct DynamicHashTable* hst, char*  key, uint64_t* val);

/************************
   ClearInHashTable() - removes entry from dynamic hash table 

   Inputs: 
      hst - pointer to a dynamic hash table
      key - string representing entry to remove

   Outputs:

   Returns:
		true if key/value pair was removed from table
		false if hash table pointer null or key not found in table

*/
bool ClearInHashTable(struct DynamicHashTable* hst, char* key);

/************************
   EntryCount() - returns the current number of entries in the hash table

   Inputs: 
      hst - pointer to a dynamic hash table

   Outputs:

   Returns:
      int equal to number of entries
      0 when hst == NULL

*/
int EntryCount(struct DynamicHashTable* hst);

/************************
   CreateHashTableIterator() - creates a cursor iterator on the heap for
		iterating through every entry in a dynamic hash table

   Inputs: 
		hst - pointer to a dynamic hash table

   Outputs:

   Returns:
      pointer to the new heap allocated iterator or NULL if allocation failed 

*/
struct HashTableIterator* CreateHashTableIterator(struct DynamicHashTable* ht);

/************************
   DestroyHashTableIterator() - frees all memory associated with hash table iterator 

   Inputs: 
		iter - pointer to a hash table iterator

   Outputs:

   Returns:

*/
void DestroyHashTableIterator(struct HashTableIterator *iter);

/************************
   HasNextEntry() - determines if hash table has another entry and 
		sets hash table iterator appropriately 

   Inputs: 
		iter - pointer to a hash table iterator

   Outputs:

   Returns:
		true if valid entry found in hash table
		false if no valid entry found

*/
bool HasNextEntry(struct HashTableIterator* iter);

/************************
   GetKey() - extracts key from current entry in hash table
		iterator 

   Inputs: 
		iter - pointer to a hash table iterator

   Outputs:

   Returns:
		string pointing to current entry's key in hash table

*/
char* GetKey(struct HashTableIterator* iter);

/************************
   GetValue() - extracts value from current entry in hash table
		iterator 

   Inputs: 
		iter - pointer to a hash table iterator

   Outputs:

   Returns:
		uint64_t representing current entry's value in hash table

*/
uint64_t GetValue(struct HashTableIterator* iter);

#endif // INC_DHT_H
