#ifndef _HASH_MAP_
#define _HASH_MAP_

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<inttypes.h>
#include <stdbool.h>
#include <murmurhash.h>

#define AVAILABLE 0
#define GROW_FACTOR 2
#define CRITICAL_LOAD_FACTOR 1

#define ALLOC_ERROR "ALLOC_ERROR: Failed to allocate memory"
#define RESIZE_ERROR "RESIZE_ERROR: Failed to resize hash_map"


typedef struct Entry {
    char* key;
    void* value;
    uint32_t probe_distance;
} entry_t;

typedef struct HashMap {
    uint32_t count;
    uint32_t capacity;
    entry_t** entries;
} hash_map_t;


hash_map_t* 
hash_map_create(uint32_t initial_capacity);

void 
hash_map_print(hash_map_t* hash_map, bool full);

void 
hash_map_free(hash_map_t* hash_map);

void
hash_map_insert_entry(hash_map_t* hash_map, char* key, void* value);

bool
hash_map_delete_entry(hash_map_t* hash_map, char* key);

bool
hash_map_has_entry(hash_map_t* hash_map, char* key);

void*
hash_map_get_entry(hash_map_t* hash_map, char* key);


#endif
