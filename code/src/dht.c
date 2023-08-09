#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dht.h>

struct Entry {
	char* key;
	uint64_t value;
};

struct DynamicHashTable {
	int count;
	int capacity;
	struct Entry* entries;
};

#define FNV_OFFSET_BASIS 2166136261u;
#define FNV_PRIME 16777619;

static uint32_t hashKey(const char* key, int len){
	uint32_t hash = FNV_OFFSET_BASIS;

	for(int i=0; i<len; i++){
		hash ^= key[i];
		hash *= FNV_PRIME;
	}

	return hash;
}

static struct Entry* findEntry(struct Entry* entries, int capacity, char* key){
	uint32_t index = hashKey(key, strlen(key)) % capacity;
	struct Entry* tombstone = NULL;

	for(;;){
		struct Entry* entry = &entries[index];
	
		if(entry->key == NULL){
			if(entry->value == 0){
				//empty entry
				return tombstone != NULL ? tombstone : entry;
			} else {
				//we found a tombstone
				if(tombstone == NULL) tombstone = entry;
			}
		} else if (strcmp(key, entry->key) == 0) {
			return entry;
		}

		index = (index + 1) % capacity;
	}
}

static void adjustTableCapacity(struct DynamicHashTable* hst, int capacity){
	struct Entry* entries = calloc(capacity , sizeof(struct Entry));

	if(hst->entries != NULL){	

		hst->count = 0;
		for(int i=0; i<hst->capacity; i++){
			struct Entry* entry = &hst->entries[i];
			if(entry->key == NULL){
				continue;
			}
	
			struct Entry* dest = findEntry(entries, capacity, entry->key);
			dest->key = entry->key;
			dest->value = entry->value;
			hst->count++;
		}
		free(hst->entries);
	}

	hst->entries = entries;
	hst->capacity = capacity; 
}

// public interface 

struct DynamicHashTable* InitHashTable(){
	struct DynamicHashTable* hst = calloc(1, sizeof(struct DynamicHashTable));
	if(hst == NULL){
		printf("Error: Unable to allocate Hash Table\r\n");
		exit(-1);
	}

	hst->count = 0;
	hst->capacity = 0;
	hst->entries = NULL;
}

void FreeHashTable(struct DynamicHashTable* hst){
	for(int i=0; i<hst->capacity; i++){
		if(hst->entries[i].key != NULL) free(hst->entries[i].key);		
	}

	hst->count = 0;
	hst->capacity = 0;

	free(hst->entries);
	hst->entries = NULL;

	free(hst);
}

bool SetInHashTable(struct DynamicHashTable* hst, char* key, uint64_t val){
	if(hst == NULL){
		printf("Erorr: Hast Table Ptr NULL\r\n");
		return false;
	}
	
	if((hst->capacity >> 1 ) < hst->count + 1){
		int oldCapacity = hst->capacity;
		int newCapacity = oldCapacity < 8 ? 8 : (oldCapacity * 2);
		adjustTableCapacity(hst, newCapacity);
	}

	struct Entry* entry = findEntry(hst->entries, hst->capacity, key);
	bool isNewKey = entry->key == NULL;
	if(isNewKey && entry->value == 0) {
		 hst->count++;

		size_t size = strlen(key) + 1;
		entry->key = calloc(size, sizeof(char));
		strncpy(entry->key, key, size);
	}
	entry->value = val;

	return isNewKey;
}

bool GetInHashTable(struct DynamicHashTable* hst, char*  key, uint64_t* val){
	if(hst == NULL){
		printf("Error Hash Table Ptr null\r\n");
		return false;
	}
	
	if(hst->count == 0) return false;

	struct Entry* entry = findEntry(hst->entries, hst->capacity, key);
	if(entry->key == NULL) return false;

	*val = entry->value;
	return true;
}

bool ClearInHashTable(struct DynamicHashTable* hst, char* key){
	if(hst == NULL){
		printf("Error Hash Table Ptr null\r\n");
		return false;
	}
	
	if(hst->count == 0) return false;

	struct Entry* entry = findEntry(hst->entries, hst->capacity, key);
	if(entry->key == NULL) return false;

	//place a tombstone in the entry
	free(entry->key);
	entry->key = NULL;
	entry->value = 0xc0de;

	return true;
}

int EntryCount(struct DynamicHashTable* hst){
	if(hst == NULL){
		printf("Error Hash Table Ptr null\r\n");
		return 0;
	}

	return hst->count;
}

// public iterator interface

struct HashTableIterator {
	char* key;
	uint64_t value;
	unsigned int indexOfPreviousEntry;
	struct DynamicHashTable* hashTable;
};

struct HashTableIterator* CreateHashTableIterator(struct DynamicHashTable* ht) {
	struct HashTableIterator* newIter = calloc(1, sizeof(struct HashTableIterator));

	newIter->hashTable = ht;
	newIter->indexOfPreviousEntry = 0;

	return newIter;
}

void DestroyHashTableIterator(struct HashTableIterator *iter){
	free(iter);
}

bool HasNextEntry(struct HashTableIterator* iter){
	
	int start = iter->indexOfPreviousEntry;
	int end = iter->hashTable->capacity;
	
	for(int i=start; i<end; i++){

		struct Entry* entry = &iter->hashTable->entries[i];
		if(entry->key == NULL) continue;
	
		//we found a valid entry so bump the cursor
		iter->indexOfPreviousEntry = i + 1;
		iter->key = entry->key;
		iter->value = entry->value;

		return true;
	}
	
	return false;
}

char* GetKey(struct HashTableIterator* iter){
	return iter->key;
}

uint64_t GetValue(struct HashTableIterator* iter){
	return iter->value;
}
