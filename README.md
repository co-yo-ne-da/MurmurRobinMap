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

### Creating a hash map

```c
#include <stdio.h>
#include <murmurrobinmap.h>

int
main(int argc, char** argv) {
  mmr_map_t hash_map = {
      .count = 0,
      .capacity = 100,
      .entries = NULL
  };

  mrrmap_alloc(&hash_map);

  printf("Created hash map with capacity %d\n", hash_map.capacity);
  // Created hash map with capacity 100;

  mmr_map_free(&hash_map);
  return 0;
}

```


### Inserting an entry
```c
#include <stdio.h>
#include <murmurrobinmap.h>


typedef struct {
  uint32_t answer;
} my_struct_t;

int
main(int argc, char** argv) {
  mmr_map_t hash_map = {
      .count = 0,
      .capacity = 100,
      .entries = NULL
  };

  mrrmap_alloc(&hash_map);

  my_struct_t* question = {
    .answer = 42
  }

  mmr_map_insert_entry(&hash_map, "answer", &question);

  mmr_map_free(&hash_map);
  return 0;
}

```


### Retrieving an entry

```c
#include <stdio.h>
#include <murmurrobinmap.h>


typedef struct {
  uint32_t answer;
} my_struct;

int
main(int argc, char** argv) {
  mmr_map_t hash_map = {
      .count = 0,
      .capacity = 100,
      .entries = NULL
  };

  mrrmap_alloc(&hash_map);

  my_struct_t question = {
    .answer = 42
  }

  mmr_map_insert_entry(&hash_map, "answer", &my_struct);

  ...

  my_struct* value = mmr_map_get_entry(&hash_map, "answer");
  if (value != NULL) {
    printf("This is an answer on ultimate question of life, the universe, and everything: %d\n", value->answer);
    // This is an answer on ultimate question of life, the universe, and everything: 42
  }

  mmr_map_free(&hash_map);
  return 0;
}

```

