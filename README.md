# MurmurRobinMap

Dependency free Hash map implementation using Robin Hood probing and Murmurhash3.

[![CI](https://github.com/co-yo-ne-da/libMurmurRobinMap/actions/workflows/ci.yml/badge.svg)](https://github.com/co-yo-ne-da/libMurmurRobinMap/actions/workflows/ci.yml)


This is still work in progress, use with care.


## Installation:

### Prerequisits

- clang v14+ (support for other C compilers to be added)

### Building from source:

```c
sudo make && make install

```

## Usage:

### Creating hash map

Interface
```c
hash_map_t* 
hash_map_create(uint32_t initial_capacity);

```

```c
hash_map_t* hash_map = hash_map_create(100);

```

Usage
```c
#include <stdio.h>
#include <murmurrobinmap.h>

int
main(int argc, char** argv) {
	hash_map_t* hash_map = hash_map_create(100);

	printf("Created hash map with capacity %d\n", hash_map->capacity);
	// Created hash map with capacity 10;

	hash_map_free(hash_map);
	return 0;
}

```
