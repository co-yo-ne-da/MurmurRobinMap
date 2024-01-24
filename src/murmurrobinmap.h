#ifndef _mmr_map_
#define _mmr_map_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>


#define AVAILABLE 0
#define GROW_FACTOR 2
#define CRITICAL_LOAD_FACTOR 0.8

#define ALLOC_ERROR "ALLOC_ERROR: Failed to allocate memory"
#define RESIZE_ERROR "RESIZE_ERROR: Failed to resize mmr_map"

typedef struct Entry {
    char* key;
    void* value;
    uint32_t probe_distance;
} entry_t;

typedef struct HashMap {
    uint32_t count;
    uint32_t capacity;
    entry_t** entries;
} mmr_map_t;


uint32_t
murmurhash3(char* key, uint32_t len, uint32_t seed);

mmr_map_t* 
mmr_map_create(uint32_t initial_capacity);

void 
mmr_map_free(mmr_map_t* mmr_map);

void
mmr_map_insert_entry(mmr_map_t* mmr_map, char* key, void* value);

bool
mmr_map_delete_entry(mmr_map_t* mmr_map, char* key);

bool
mmr_map_has_entry(mmr_map_t* mmr_map, char* key);

void*
mmr_map_get_entry(mmr_map_t* mmr_map, char* key);


#endif
