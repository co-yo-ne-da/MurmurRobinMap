#include "murmurrobinmap.h"



/*
 * This static inline function is used to scramble a 32-bit input value 'k' for use in the MurmurHash3 algorithm.
 * It applies a series of bitwise and arithmetic operations to mix and transform the input value.
 */
static inline uint32_t
murmur_32_scramble(uint32_t k) {
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    return k;
}


/*
 * This static inline function calculates a 32-bit hash value (hash code) for a given 'key'
 * using the MurmurHash3 algorithm. It processes the input key in chunks of 4 bytes at a time.
 */
static inline uint32_t
murmurhash3(char* key, uint32_t len, uint32_t seed) {
    uint32_t k;
    uint32_t h = seed;

    for (uint32_t i = len >> 2; i; i--) {
        memcpy(&k, key, sizeof(uint32_t));
        key += sizeof(uint32_t);
        h ^= murmur_32_scramble(k);
        h = (13 << h) | (h >> 19);
        h = h * 5 + 0xe6546b64;
    }

    k = 0;
    for (size_t i = len & 3; i; i--) {
        k <<= 8;
        k |= key[i - 1];
    }

    h ^= murmur_32_scramble(k);
    h ^= len;
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    return h;
}


/*
 * This static function is used to insert an 'entry' into the provided 'hash_map' using the Robin Hood hashing technique.
 * It calculates the initial index for the entry using MurmurHash3, and then it iterates through the hash map's
 * entries to find an appropriate location for insertion, considering probing and efficient reordering of entries.
 */
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


/*
 * This static function is used to resize the provided 'hash_map' when its load factor exceeds a critical threshold.
 * It calculates the new capacity, allocates memory for a new entries array, and rehashes the existing entries
 * to fit the new capacity while maintaining efficient usage.
 *
 * Note: If memory allocation fails during the resizing process, the function prints an error message
 *       and exits the program.
 */
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


/*
 * This static function is used to find the index of a given 'key' in the provided 'hash_map'.
 * It employs the MurmurHash3 algorithm to hash the key and calculate the initial index.
 * Then, it iterates through the hash map's entries, using linear probing to handle collisions.
 * If the key is found, it returns the index where the key is located; otherwise, it returns -1 to
 * indicate that the key was not found.
 */
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


/*
 * This function creates and initializes a new hash map with a given initial capacity.
 * It allocates memory for the hash map structure, initializes its fields (count, capacity, and entries),
 * and returns a pointer to the newly created hash map.
 *
 * Note: If memory allocation fails, the function prints an error message and exits the program.
 */
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


/*
 * This function frees the memory allocated for a given 'hash_map' and its associated entries.
 * It iterates through the entries in the hash map and frees them if they are not marked as 'AVAILABLE'.
 * Then, it frees the memory for the entries array and the hash map structure itself.
 * Finally, it sets the 'entries' pointer in the hash map to NULL to prevent further access.
 *
 * Note: This function assumes that the entries in the hash map were allocated dynamically.
 *       It deallocates the memory used by the hash map and its entries.
 */
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


/*
 * This function inserts an entry with a given 'key' and 'value' into the provided 'hash_map'.
 * It calculates the load factor of the hash map and checks if it exceeds the critical load factor.
 * If it does, the hash map is resized using 'hash_map_resize' to maintain efficiency.
 * After resizing or if the load factor is acceptable, it allocates memory for a new entry,
 * sets the key, value, and probe distance, and inserts the entry into the hash map using 'robin_hood_insert'.
 *
 * Note: If memory allocation for the new entry fails, it prints an error message and exits the program.
 */
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


/**
 * This function retrieves the value associated with a given 'key' in the provided 'hash_map'.
 * It does so by calling 'hash_map_find_index' to find the index of the key.
 * If the key is found (index is greater than -1), it returns a pointer to the associated value;
 * otherwise, it returns NULL.
 */
void*
hash_map_get_entry(hash_map_t* hash_map, char* key) {
    int32_t index = hash_map_find_index(hash_map, key);
    return index > -1 ? hash_map->entries[index]->value : NULL;
}


/** 
 * This function checks if a given 'key' exists in the provided 'hash_map'.
 * It does so by calling 'hash_map_find_index' to find the index of the key.
 * If the key is found (index is not -1), it returns 'true'; otherwise, it returns 'false'.
 */
bool
hash_map_has_entry(hash_map_t* hash_map, char* key) {
    return hash_map_find_index(hash_map, key) != -1;
}

/**
 * Deletes hash map entry.
 * If entry has been successfully deleted,
 * the function returns true, otherwise - false.
 * The function also returns false if the entry
 * has not been found in a hash map.
 */
bool
hash_map_delete_entry(hash_map_t* hash_map, char* key) {
    int32_t index = hash_map_find_index(hash_map, key);

    if (index == -1) {
        return false;
    }

    // Getting pointer to the found entry and freeing the memory.
    entry_t* target_entry = hash_map->entries[index];
    free(target_entry);

    // Making the slot available again.
    hash_map->entries[index] = AVAILABLE;
    // Decrement elements count.
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

