#include "hash_map.h"


static uint32_t
calculate_hash(char* key, uint32_t size) {
    return murmurhash(key, (uint32_t)strlen(key), 0) % size;
}

static entry_t*
create_entry(char* key, void* value) {
	// FIXME: Error handling
	entry_t* entry = malloc(sizeof(entry_t));
	entry->key = key;
	entry->value = value;
	entry->probe_distance = 0;
	return entry;
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
		} else if (strcmp(hash_map->entries[i]->key, entry->key) == 0) {
			hash_map->entries[i]->value = entry->value;
			break;
		} else if (hash_map->entries[i]->probe_distance < testee->probe_distance) {
			entry_t* tmp = hash_map->entries[i];
			hash_map->entries[i] = testee;
			testee = tmp;
			continue;
		} else {
			testee->probe_distance++;
			continue;
		}
	}

	return hash_map;
}

static hash_map_t*
hash_map_resize(hash_map_t* hash_map) {
	uint32_t new_capacity = hash_map->capacity * GROW_FACTOR;
	entry_t** new_entries = calloc(hash_map->capacity, sizeof(entry_t));
	if (new_entries == NULL) {
		perror(RESIZE_ERROR);
		return NULL;
	}

	entry_t** legacy_entries = hash_map->entries;

	hash_map->entries = new_entries;
	hash_map->capacity = new_capacity;

	for (uint32_t i = 0; i < hash_map->capacity; i++) {
		if (legacy_entries[i] != AVAILABLE) {
			legacy_entries[i]->probe_distance = 0;
			robin_hood_insert(hash_map, legacy_entries[i]);
		}
	}

	free(legacy_entries);

	return hash_map;
}


hash_map_t*
hash_map_create(uint32_t initial_capacity) {
	hash_map_t* map = malloc(sizeof(hash_map_t));
	if (map == NULL) {
		perror(ALLOC_ERROR);
		return NULL;
	}

	map->count = 0;
	map->capacity = initial_capacity;
	map->entries = calloc(initial_capacity, sizeof(entry_t));
	return map;
}

void
hash_map_print(hash_map_t* hash_map, bool full) {
	for (uint32_t i = 0; i < hash_map->capacity; i++) {
		if (hash_map->entries[i] == AVAILABLE) {
			if (full) {
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
		if (hash_map->entries[i] == AVAILABLE) {
			free(hash_map->entries[i]);
		}
	}

	free(hash_map);
}


void
hash_map_insert_entry(hash_map_t* hash_map, char* key, void* value) {
	float load_factor = (float)hash_map->count / (float)hash_map->capacity;
	if (CRITICAL_LOAD_FACTOR <= load_factor) {
		hash_map_t* resized_map = hash_map_resize(hash_map);
		if (resized_map == NULL) {
			return;
		}

		hash_map = resized_map;
	}

	robin_hood_insert(hash_map, create_entry(key, value));
}

void*
hash_map_get_entry(hash_map_t* hash_map, char* key) {
	uint32_t index = calculate_hash(key, hash_map->capacity);
	for (uint32_t i = index; i < hash_map->capacity; i = (i + 1) % hash_map->capacity) {
		if (strcmp(hash_map->entries[i]->key, key) == 0) {
			return hash_map->entries[i]->value;
		} 
	}

	return NULL;
}

