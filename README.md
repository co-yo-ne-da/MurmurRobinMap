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

Usage
```c
hash_map_t* hash_map = hash_map_create(100);

```

Example
```c
#include <stdio.h>
#include <murmurrobinmap.h>

int
main(int argc, char** argv) {
  hash_map_t* hash_map = hash_map_create(100);

  printf("Created hash map with capacity %d\n", hash_map->capacity);
  // Created hash map with capacity 100;

  hash_map_free(hash_map);
  return 0;
}

```


### Inserting an entry

Interface

```c
void
hash_map_insert_entry(hash_map_t* hash_map, char* key, void* value);

```

Usage
```c
hash_map_insert_entry(hash_map, "key", "value");

my_struct* s = malloc(sizeof(my_struct));
s->something = 42;
hash_map_insert_entry(hash_map, "my_struct", s);
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
  hash_map_t* hash_map = hash_map_create(100);

  my_struct* s = malloc(sizeof(my_struct));
  s->answer = 42;

  hash_map_insert_entry(hash_map, "answer", my_struct);

  hash_map_free(hash_map);
  return 0;
}

```


### Retrieving an entry

Interface

```c
void*
hash_map_get_entry(hash_map_t* hash_map, char* key);

```

Usage
```c
char* value = (char*)hash_map_get_entry(hash_map, "key");

my_struct* struct_value = (my_struct*)hash_map_get_entry(hash_map, "my_struct");

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
  hash_map_t* hash_map = hash_map_create(100);

  my_struct* s = malloc(sizeof(my_struct));
  s->answer = 42;

  hash_map_insert_entry(hash_map, "answer", my_struct);

  ...

  my_struct* s2 = hash_map_get_entry(hash_map, "answer");
  if (s2 != NULL) {
    printf("This is an answer on ultimate question of life, the universe, and everything: %d\n", s2->answer);
    // This is an answer on ultimate question of life, the universe, and everything: 42
  }

  free(s);
  hash_map_free(hash_map);
  return 0;
}

```

