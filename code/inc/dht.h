#ifndef INC_DHT_H
#define INC_DHT_H

#include <stdbool.h>

/*
	Dynamic hash table

	When to use:
		use when

*/

typedef struct DynamicHashTable Dht;

Dht* InitHashTable();
void FreeHashTable(Dht* hst);

bool GetEntryInHashTable(struct DynamicHashTable* hst, char*  key, int* val);
bool SetEntryInHashTable(struct DynamicHashTable* hst, char* key, int val);
bool ClearEntryInHashTable(struct DynamicHashTable* hst, char* key);

#endif // INC_DHT_H
