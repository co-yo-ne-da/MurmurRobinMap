# MurmurRobinMap

Dependency free Hash map implementation using Robin Hood probing and Murmurhash3.

[![CI](https://github.com/co-yo-ne-da/libMurmurRobinMap/actions/workflows/ci.yml/badge.svg)](https://github.com/co-yo-ne-da/libMurmurRobinMap/actions/workflows/ci.yml)


This is still work in progress, use with care.

## References

### Robin hood probing
 - https://www.cs.cornell.edu/courses/JavaAndDS/files/hashing_RobinHood.pdf
 - https://thenumb.at/Hashtables/#robin-hood-linear-probing

### Murmurhash3
  - https://en.wikipedia.org/wiki/MurmurHash



## Installation:

### Prerequisites

- clang v14+ or gcc

### Building from source:

##### With clang (default)

```c
make && sudo make install

```

##### With gcc

```c
CC=gcc make && sudo make install

```

## Usage:

### Creating hash map

Interface
```c
mmr_map_t* 
mmr_map_create(uint32_t initial_capacity);

```

Usage
```c
mmr_map_t* hash_map = mmr_map_create(100);

```

Example
```c
#include <stdio.h>
#include <murmurrobinmap.h>

int
main(int argc, char** argv) {
  mmr_map_t* hash_map = mmr_map_create(100);

  printf("Created hash map with capacity %d\n", hash_map->capacity);
  // Created hash map with capacity 100;

  mmr_map_free(hash_map);
  return 0;
}

```


### Inserting an entry

Interface

```c
void
mmr_map_insert_entry(mmr_map_t* mmr_map, char* key, void* value);

```

Usage
```c
mmr_map_insert_entry(hash_map, "key", "value");

my_struct* s = malloc(sizeof(my_struct));
s->something = 42;
mmr_map_insert_entry(hash_map, "my_struct", s);
```

Example
```c
#include <stdio.h>
#include <murmurrobinmap.h>


typedef struct {
  uint32_t answer;
} my_struct;

int
main(int argc, char** argv) {
  mmr_map_t* hash_map = mmr_map_create(100);

  my_struct* s = malloc(sizeof(my_struct));
  s->answer = 42;

  mmr_map_insert_entry(hash_map, "answer", my_struct);

  mmr_map_free(hash_map);
  return 0;
}

```


### Retrieving an entry

Interface

```c
void*
mmr_map_get_entry(mmr_map_t* mmr_map, char* key);

```

Usage
```c
char* value = (char*)mmr_map_get_entry(hash_map, "key");

my_struct* struct_value = (my_struct*)mmr_map_get_entry(hash_map, "my_struct");

```

Example
```c
#include <stdio.h>
#include <murmurrobinmap.h>


typedef struct {
  uint32_t answer;
} my_struct;

int
main(int argc, char** argv) {
  mmr_map_t* hash_map = mmr_map_create(100);

  my_struct* s = malloc(sizeof(my_struct));
  s->answer = 42;

  mmr_map_insert_entry(hash_map, "answer", my_struct);

  ...

  my_struct* s2 = mmr_map_get_entry(hash_map, "answer");
  if (s2 != NULL) {
    printf("This is an answer on ultimate question of life, the universe, and everything: %d\n", s2->answer);
    // This is an answer on ultimate question of life, the universe, and everything: 42
  }

  free(s);
  mmr_map_free(hash_map);
  return 0;
}

```

