#include "hash_map.h"


static uint32_t
calculate_hash(char* key, uint32_t size) {
    return murmurhash(key, (uint32_t)strlen(key), 0) % size;
}


static hash_map_t*
hash_map_resize(hash_map_t* hash_map) {
	uint32_t new_capacity = hash_map->capacity * GROW_FACTOR;
	entry_t** new_entries = calloc(new_capacity, sizeof(entry_t));

	for (uint32_t i = 0; i < hash_map->capacity; i++) {
		entry_t* entry = hash_map->entries[i];
		if (entry != AVAILABLE) {
			uint32_t hash = calculate_hash(entry->key, new_capacity);
			new_entries[hash] = entry;
		}
	}

	free(hash_map->entries);
	hash_map->capacity = new_capacity;
	hash_map->entries = new_entries;

	return hash_map;
}


hash_map_t*
hash_map_create(uint32_t initial_capacity) {
	hash_map_t* map = malloc(sizeof(hash_map_t));
	if (map == NULL) {
		perror("Failed to allocated memory for hash_map");
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

		printf("%d: %s -> %p\n", i, hash_map->entries[i]->key, hash_map->entries[i]->value);
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
		hash_map = hash_map_resize(hash_map);
	}

	uint32_t index = calculate_hash(key, hash_map->capacity);

	for (uint32_t i = index; i < hash_map->capacity; i = (i + 1) % hash_map->capacity) {
		if (hash_map->entries[i] == AVAILABLE) {
			entry_t* entry = malloc(sizeof(entry_t));
			entry->key = key;
			entry->value = value;
			hash_map->entries[i] = entry;
			hash_map->count++;
			break;
		} else if (strcmp(hash_map->entries[i]->key, key) == 0) {
			hash_map->entries[i]->value = value;
			break;
		} else {
			continue;
		}
	}
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

