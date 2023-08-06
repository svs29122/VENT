#ifndef INC_DHT_H
#define INC_DHT_H

#include <stdbool.h>

/*
	Dynamic hash table

	When to use:
		use when

*/

struct Entry {
	char* key;
	int value;
};

struct DynamicHashTable* InitHashTable();
void FreeHashTable(struct DynamicHashTable* hst);

bool GetInHashTable(struct DynamicHashTable* hst, char*  key, int* val);
bool SetInHashTable(struct DynamicHashTable* hst, char* key, int val);
bool ClearInHashTable(struct DynamicHashTable* hst, char* key);

struct HashTableIterator* CreateHashTableIterator(struct DynamicHashTable* ht);
void DestroyHashTableIterator(struct HashTableIterator *iter);
struct Entry* GetNextEntry(struct HashTableIterator* iter);

#endif // INC_DHT_H
