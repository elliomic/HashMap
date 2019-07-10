#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "hashMap.h"

struct hashLink {
	KeyType key; /*the key is what you use to look up a hashLink*/
	ValueType value; /*the value stored with the hashLink, a pointer to int in the case of concordance*/
	struct hashLink * next;
};
typedef struct hashLink hashLink;

struct hashMap {
    hashLink ** table; /*array of pointers to hashLinks*/
    int tableSize; /*number of buckets in the table*/
    int count; /*number of hashLinks in the table*/
};
typedef struct hashMap hashMap;

int stringHash2(char * str)
{
	int i;
	int r = 0;
	for (i = 0; str[i] != '\0'; i++)
		r += (i+1) * str[i];
	return r;
}

/* initialize the supplied hashMap struct*/
void _initMap (struct hashMap * ht, int tableSize)
{
	int index;
	if(ht == NULL)
		return;
	ht->table = (hashLink**)malloc(sizeof(hashLink*) * tableSize);
	ht->tableSize = tableSize;
	ht->count = 0;
	for(index = 0; index < tableSize; index++)
		ht->table[index] = NULL;
}

/* allocate memory and initialize a hash map*/
hashMap *createMap(int tableSize) {
	assert(tableSize > 0);
	hashMap *ht;
	ht = malloc(sizeof(hashMap));
	assert(ht != 0);
	_initMap(ht, tableSize);
	return ht;
}

/*
  Free all memory used by the buckets.
*/
void _freeLinks (struct hashLink **table, int tableSize)
{	
	hashLink * prev;
	hashLink * current;
	int c;
	c = 0;
	while (c < tableSize) {
		current = table[c];
		while (current != NULL) {
			prev = current;
			current = current->next;
			free(prev->key);
			free(prev->value);
			free(prev);
		}
		++c;
	}
}

/* Deallocate buckets, table, and hashTable structure itself.*/
void deleteMap(hashMap *ht) {
	
	
	assert(ht!= 0);
	/* Free all memory used by the buckets */
	_freeLinks(ht->table, ht->tableSize);
	/* Free the array of buckets */
	free(ht->table);
	/* free the hashMap struct */
	free(ht);
}

/*
  Resizes the hash table to be the size newTableSize
*/
void _setTableSize(struct hashMap * ht, int newTableSize)
{
	hashLink ** tableCopy;
	hashLink * prev;
	hashLink * current;
	int c, max;
	tableCopy = ht->table;
	c = 0;
	max = capacity(ht);
	_initMap(ht, newTableSize);
	while (c < max) {
		current = tableCopy[c];
		while (current != NULL) {
			prev = current;
			current = current->next;
			insertMap(ht, prev->key, prev->value);
			free(prev);
		}
		++c;
	}
	free(tableCopy);
}

/*
  insert the following values into a hashLink  
*/
void insertMap (struct hashMap * ht, KeyType k, ValueType v)
{
  	int hash, bucket;
	hashLink * link;
	hashLink * current;
	hash = stringHash2(k);
	bucket = hash % ht->tableSize;
	if (ht->table[bucket] == NULL) {
		link = malloc(sizeof(hashLink));
		link->key = k;
		link->value = v;
		ht->table[bucket] = link;
		++ht->count;
	} else {
		current = ht->table[bucket];
		while (current->next != NULL && !strcmp(current->next->key, k)) {
			current = current->next;
		}
		if (current->next == NULL) {
			link = malloc(sizeof(hashLink));
			link->key = k;
			current->next = link;
			++ht->count;
		}
		free(current->next->value);
		current->next->value = v;
	}
	if (tableLoad(ht) >= LOAD_FACTOR_THRESHOLD) {
		_setTableSize(ht, size(ht) * 2);
	}
}

/*
  this returns the value (which is void*) stored in a hashLink specified by the key k.
  
  if the supplied key is not in the hashtable return NULL.
*/
ValueType atMap (struct hashMap * ht, KeyType k)
{
	int hash, bucket;
	hashLink * current;
	hash = stringHash2(k);
	bucket = hash % capacity(ht);
	if (ht->table[bucket] != NULL) {
		current = ht->table[bucket];
		while (current->next != NULL) {
			if (!strcmp(current->key, k)) {
				return current->value;
			}
			current = current->next;
		}
	}
	return 0;
}

/*
  a simple yes/no if the key is in the hashtable.
  0 is no, all other values are yes.
*/
int containsKey (struct hashMap * ht, KeyType k)
{
	return atMap(ht, k);
}

/*
  find the hashlink for the supplied key and remove it, also freeing the memory
  for that hashlink. it is not an error to be unable to find the hashlink
*/
void removeKey (struct hashMap * ht, KeyType k)
{
	if (containsKey(ht, k)) {
		struct hashLink * prev;
		struct hashLink * current;
		int hash, bucket;
		prev = NULL;
		hash = stringHash2(k);
		bucket = hash % capacity(ht);
		if (ht->table[bucket] != NULL) {
			current = ht->table[bucket];
			while (current->next != NULL) {
				if (!strcmp(prev->key, k)) {
					if(prev == NULL) {
						ht->table[bucket] = current->next;
					} else {
						prev->next = current->next;
					}
					free(prev->key);
					free(prev->value);
					free(prev);
					return;
				}
				prev = current;
				current = current->next;
			}
		}
	}
	return;
}

/*
  returns the number of hashLinks in the table
*/
int size (struct hashMap *ht)
{
	return ht->count;
}

/*
  returns the number of buckets in the table
*/
int capacity(struct hashMap *ht)
{
	return ht->tableSize;
}

/*
  returns the number of empty buckets in the table, these are buckets which have
  no hashlinks hanging off of them.
*/
int emptyBuckets(struct hashMap *ht)
{
	int c, total;
	c = total= 0;
	while (c < capacity(ht)) {
		if (ht->table[c] == NULL) {
			++total;
		}
		++c;
	}
	return 0;
}

/*
  returns the ratio of: (number of hashlinks) / (number of buckets)
  
  this value can range anywhere from zero (an empty table) to more then 1, which
  would mean that there are more hashlinks then buckets (but remember hashlinks
  are like linked list nodes so they can hang from each other)
*/
float tableLoad(struct hashMap *ht)
{
	return ((float)size(ht) / (float)capacity(ht));
}

/* print the hashMap */
void printMap (struct hashMap * ht, keyPrinter kp, valPrinter vp)
{
	int i;
	struct hashLink *temp;
	for(i = 0;i < capacity(ht); i++){
		temp = ht->table[i];
		if(temp != 0) {
			printf("\nBucket Index %d -> ", i);
		}
		while(temp != 0){
			printf("Key:");
			(*kp)(temp->key);
			printf("| Value: ");
			(*vp)(temp->value);
			printf(" -> ");
			temp=temp->next;
		}
	}
}

/* print the keys/values ..in linear form, no buckets */
void printKeyValues (struct hashMap * ht, keyPrinter kp, valPrinter vp)
{
	int i;
	struct hashLink *temp;
	for(i = 0;i < capacity(ht); i++){
		temp = ht->table[i];
		while(temp != 0){
			(*kp)(temp->key);
			printf(":");
			(*vp)(temp->value);
			printf("\n");
			temp=temp->next;
		}
	}
}
