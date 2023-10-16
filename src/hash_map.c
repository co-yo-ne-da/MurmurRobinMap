#include "hash_map.h"


static uint32_t
calculate_hash(char* key, uint32_t size) {
    return murmurhash(key, (uint32_t)strlen(key), 0) % size;
}

static hash_map_t*
robin_hood_insert(hash_map_t* hash_map, entry_t* entry) {
	entry_t* testee = entry;
	uint32_t index = calculate_hash(entry->key, hash_map->capacity);

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
	uint32_t index = calculate_hash(key, hash_map->capacity);

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
hash_map_print(hash_map_t* hash_map, bool print_full_table) {
	for (uint32_t i = 0; i < hash_map->capacity; i++) {
		if (hash_map->entries[i] == AVAILABLE) {
			if (print_full_table) {
				printf("%d: [EMPTY SLOT]\n", i);
			}
			continue;
		}

		entry_t* entry = hash_map->entries[i];
		printf("%d: %s -> %p ; PD: %d \n", i, entry->key, entry->value, entry->probe_distance);
	}
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


/**
 * Deletes hash map entry.
 * If entry has been successfully deleted,
 * returns true, otherwise - false.
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
	* 	- an available slot met
	* 	- the probe distance of the given element is 0.
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

