#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "hash_map.h"


int
main(int argc, char** argv) {
    hash_map_t* map = hash_map_create(30);


    uint32_t age = 39;

    hash_map_insert_entry(map, "name", "Andrew");
    hash_map_insert_entry(map, "name1", "Andrew2");
    hash_map_insert_entry(map, "name2", "Andrew3");

    hash_map_insert_entry(map, "name3", "Andrew4");
    hash_map_insert_entry(map, "name4", "Andrew5");
    hash_map_insert_entry(map, "name5", "Andrew6");
    hash_map_insert_entry(map, "name6", "Andrew7");
    hash_map_insert_entry(map, "age", &age);
    hash_map_insert_entry(map, "name7", "Andrew8");

    hash_map_insert_entry(map, "name8", "Andrew9");
    hash_map_insert_entry(map, "sleeping", "normally");
    hash_map_insert_entry(map, "occupation", "Computer programmer");

    hash_map_insert_entry(map, "name", "Andrew");
    hash_map_insert_entry(map, "name10", "Andrew2");
    hash_map_insert_entry(map, "name20", "Andrew3");

    hash_map_insert_entry(map, "name33", "Andrew4");
    hash_map_insert_entry(map, "name40", "Andrew5");
    hash_map_insert_entry(map, "name50", "Andrew6");
    hash_map_insert_entry(map, "name64", "Andrew7");
    hash_map_insert_entry(map, "age2", &age);
    hash_map_insert_entry(map, "name73", "Andrew8");

    hash_map_insert_entry(map, "name87", "Andrew9");
    hash_map_insert_entry(map, "sleeping6", "normally");
    hash_map_insert_entry(map, "occupation4", "Computer programmer");

    printf("---\n");
    
    hash_map_print(map, true);
    hash_map_free(map);

    return EXIT_SUCCESS;
}
