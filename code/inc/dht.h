#ifndef INC_DHT_H
#define INC_DHT_H

#include <stdbool.h>

/*
	Dynamic hash table

	When to use:
		use when

*/

struct DynamicHashTable* InitHashTable();
void FreeHashTable(struct DynamicHashTable* hst);

bool GetInHashTable(struct DynamicHashTable* hst, char*  key, int* val);
bool SetInHashTable(struct DynamicHashTable* hst, char* key, int val);
bool ClearInHashTable(struct DynamicHashTable* hst, char* key);
int EntryCount(struct DynamicHashTable* hst);

struct HashTableIterator* CreateHashTableIterator(struct DynamicHashTable* ht);
void DestroyHashTableIterator(struct HashTableIterator *iter);

bool HasNextEntry(struct HashTableIterator* iter);
char* GetKey(struct HashTableIterator* iter);
int GetValue(struct HashTableIterator* iter);

#endif // INC_DHT_H
