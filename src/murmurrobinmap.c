#include "murmurrobinmap.h"


static inline uint32_t
murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}


inline uint32_t
murmurhash3(char* key, uint32_t len, uint32_t seed) {
    if (key == NULL || len == 0) return 0;
    uint32_t hash = seed;

    const size_t nblocks = len / sizeof(uint32_t);
    const uint32_t* blocks = (uint32_t*) key;
    const uint8_t* tail = (uint8_t*) (key + nblocks * 4);

    for (size_t i = 0; i < nblocks; i++) {
        hash ^= murmur_32_scramble(*blocks++);
        hash = (hash << 13) | (hash >> 19);
        hash = (hash * 5) + 0xe6546b64;
    }

    uint32_t k = 0;
    switch (len & 3) {
        case 3:
            k ^= tail[2] << 16;
        case 2:
            k ^= tail[1] << 8;
        case 1:
            k ^= tail[0];
            hash ^= murmur_32_scramble(k);
    }

    hash ^= len;
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}


static hash_map_t*
robin_hood_insert(hash_map_t* hash_map, entry_t* entry) {
    entry_t* testee = entry;
    uint32_t index = murmurhash3(entry->key, (uint32_t)strlen(entry->key), 0) % hash_map->capacity;

    for (uint32_t i = index; i < hash_map->capacity; i = (i + 1) % hash_map->capacity) {
        if (hash_map->entries[i] == AVAILABLE) {
            hash_map->entries[i] = testee;
            hash_map->count++;
            break;
        }

        if (strcmp(hash_map->entries[i]->key, entry->key) == 0) {
            hash_map->entries[i]->value = entry->value;
            break;
        } 

        if (hash_map->entries[i]->probe_distance < testee->probe_distance) {
            entry_t* tmp = hash_map->entries[i];
            hash_map->entries[i] = testee;
            testee = tmp;
            testee->probe_distance = 0;
            continue;
        } 

        testee->probe_distance++;
    }

    return hash_map;
}


static void
hash_map_resize(hash_map_t* hash_map) {
    uint32_t legacy_capacity = hash_map->capacity;
    uint32_t new_capacity = legacy_capacity * GROW_FACTOR;

    entry_t** new_entries = calloc(new_capacity, sizeof(entry_t*));
    if (new_entries == NULL) {
        perror(ALLOC_ERROR);
        exit(1);
    }

    entry_t** legacy_entries = hash_map->entries;
    hash_map->entries = new_entries;
    hash_map->capacity = new_capacity;
    hash_map->count = 0;

    for (uint32_t i = 0; i < legacy_capacity; i++) {
        if (legacy_entries[i] != AVAILABLE) 
            robin_hood_insert(hash_map, legacy_entries[i]);
    }

    free(legacy_entries);
}


static int32_t
hash_map_find_index(hash_map_t* hash_map, char* key) {
    uint32_t index = murmurhash3(key, (uint32_t)strlen(key), 0) % hash_map->capacity;

    for (int32_t i = index; i < hash_map->capacity; i = (i + 1) % hash_map->capacity) {
        if (hash_map->entries[i] == AVAILABLE) {
            return -1;
        }

        if (strcmp(hash_map->entries[i]->key, key) == 0) {
            return i;
        }
    }

    return -1;
}


hash_map_t*
hash_map_create(uint32_t initial_capacity) {
    hash_map_t* map = malloc(sizeof(hash_map_t));
    if (map == NULL) {
        perror(ALLOC_ERROR);
        exit(1);
    }

    map->count = 0;
    map->capacity = initial_capacity;
    map->entries = calloc(initial_capacity, sizeof(entry_t*));
    return map;
}


void
hash_map_free(hash_map_t* hash_map) {
    for (uint32_t i = 0; i < hash_map->capacity; i++) {
        if (hash_map->entries[i] != AVAILABLE) {
            free(hash_map->entries[i]);
        }
    }

    free(hash_map->entries);
    free(hash_map);

    hash_map->entries = NULL;
}


void
hash_map_insert_entry(hash_map_t* hash_map, char* key, void* value) {
    float load_factor = (float)hash_map->count / (float)hash_map->capacity;
    if (CRITICAL_LOAD_FACTOR <= load_factor) {
        hash_map_resize(hash_map);
    }

    entry_t* entry = malloc(sizeof *entry);
    if (entry == NULL) {
        perror(ALLOC_ERROR);
        exit(1);
    }

    entry->key = key;
    entry->value = value;
    entry->probe_distance = 0;
    robin_hood_insert(hash_map, entry);
}


void*
hash_map_get_entry(hash_map_t* hash_map, char* key) {
    int32_t index = hash_map_find_index(hash_map, key);
    return index > -1 ? hash_map->entries[index]->value : NULL;
}


bool
hash_map_has_entry(hash_map_t* hash_map, char* key) {
    return hash_map_find_index(hash_map, key) != -1;
}


bool
hash_map_delete_entry(hash_map_t* hash_map, char* key) {
    int32_t index = hash_map_find_index(hash_map, key);

    if (index == -1) {
        return false;
    }

    entry_t* target_entry = hash_map->entries[index];
    free(target_entry);

    hash_map->entries[index] = AVAILABLE;
    hash_map->count--;

    /* Moving right side neighbours to the left one by one until either:
    *   - an available slot met
    *   - the probe distance of the given element is 0.
    */
    for (uint32_t i = (uint32_t)index + 1; i < hash_map->capacity; i = (i + 1) % hash_map->capacity) {
        if (hash_map->entries[i] == AVAILABLE || hash_map->entries[i]->probe_distance == 0) break;

        uint32_t ti = i == 0 ? hash_map->capacity - 1 : i - 1;
        hash_map->entries[ti] = hash_map->entries[i];
        hash_map->entries[ti]->probe_distance -= 1;
        hash_map->entries[i] = AVAILABLE;
    }

    return true;
}

