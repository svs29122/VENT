#ifndef INC_DHT_H
#define INC_DHT_H

#include <stdbool.h>
#include <stdint.h>

/*
	Dynamic hash table

	When to use:
		use when

*/

struct DynamicHashTable* InitHashTable();
void FreeHashTable(struct DynamicHashTable* hst);

bool GetInHashTable(struct DynamicHashTable* hst, char*  key, uint64_t* val);
bool SetInHashTable(struct DynamicHashTable* hst, char* key, uint64_t val);
bool ClearInHashTable(struct DynamicHashTable* hst, char* key);
int EntryCount(struct DynamicHashTable* hst);

struct HashTableIterator* CreateHashTableIterator(struct DynamicHashTable* ht);
void DestroyHashTableIterator(struct HashTableIterator *iter);

bool HasNextEntry(struct HashTableIterator* iter);
char* GetKey(struct HashTableIterator* iter);
uint64_t GetValue(struct HashTableIterator* iter);

#endif // INC_DHT_H
