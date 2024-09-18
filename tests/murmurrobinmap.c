#include <stdlib.h>
#include <check.h>
#include <string.h>
#include <murmurrobinmap.c>


entry_t*
create_entry(char* key, void* value) {
    entry_t* entry = malloc(sizeof *entry);
    entry->key = key;
    entry->value = value;
    entry->probe_distance = 0;
    return entry;
}


START_TEST(test_mmr_map_free) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };

    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    mmr_map_free(&map);
    ck_assert_ptr_null(map.entries);
}
END_TEST


START_TEST(test_mmr_map_create) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };

    mmr_map_alloc(&map);

    ck_assert_ptr_nonnull(map.entries);
    ck_assert_uint_eq(map.capacity, 10);
    ck_assert_uint_eq(map.count, 0);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_mmr_map_insert_entry_upates_count) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    mmr_map_insert_entry(&map, "hello", "world");
    ck_assert_uint_eq(map.count, 1);

    mmr_map_insert_entry(&map, "another", "entry");
    ck_assert_uint_eq(map.count, 2);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_mmr_map_insert_entry_adds_an_entry) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    mmr_map_insert_entry(&map, "hello", "world");
    
    uint32_t index = mmr_map_find_index(&map, "hello");
    entry_t entry = map.entries[index];
    ck_assert_str_eq((char*)entry.value, "world");

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_mmr_map_resize) {
    uint32_t capacity = 10;
    mmr_map_t map = {
        .count = 0,
        .capacity = capacity,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    mmr_map_resize(&map);

    ck_assert_uint_eq(map.capacity, capacity * GROW_FACTOR);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_mmr_map_autoresize) {
    uint32_t capacity = 10;
    mmr_map_t map = {
        .count = 0,
        .capacity = capacity,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    char* template = "dgy63jfgew4qafgkdsfskfj";
    for (size_t i = 0; i < 10; i++) {
        char* key = malloc(i * sizeof(char));
        strncpy(key, template, i);
        mmr_map_insert_entry(&map, key, key);
    }

    ck_assert_uint_eq(map.count, 10);
    ck_assert_uint_eq(map.capacity, capacity * GROW_FACTOR);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_robin_hood_insert_probe_distance) {
    uint32_t capacity = 10;
    mmr_map_t map = {
        .count = 0,
        .capacity = capacity,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    entry_t* entry = create_entry("test11", "hello");
    entry_t* test_entry = create_entry("test12", "world");
    entry_t* test_entry2 = create_entry("test16", "test");

    // Make sure test keys produce the same hash value
    ck_assert_uint_eq(
        murmurhash3("test11", 6, 0) % capacity,
        murmurhash3("test12", 6, 0) % capacity
    );
    ck_assert_uint_eq(
        murmurhash3("test12", 6, 0) % capacity,
        murmurhash3("test16", 6, 0) % capacity
    );
    ck_assert_uint_eq(
        murmurhash3("test11", 6, 0) % capacity,
        murmurhash3("test16", 6, 0) % capacity
    );

    robin_hood_insert(&map, entry->key, entry->value);
    uint32_t index = mmr_map_find_index(&map, "test11");
    ck_assert_str_eq((char*)(map.entries[index].value), "hello");
    ck_assert_uint_eq(map.entries[index].probe_distance, 0);

    robin_hood_insert(&map, test_entry->key, test_entry->value);
    uint32_t index1 = mmr_map_find_index(&map, "test12");
    ck_assert_uint_eq(index1, index + 1);
    ck_assert_str_eq((char*)(map.entries[index1].value), "world");
    ck_assert_uint_eq(map.entries[index1].probe_distance, 1);

    robin_hood_insert(&map, test_entry2->key, test_entry2->value);
    uint32_t index2 = mmr_map_find_index(&map, "test16");
    ck_assert_uint_eq(index2, index1 + 1);
    ck_assert_str_eq((char*)(map.entries[index2].value), "test");
    ck_assert_uint_eq(map.entries[index2].probe_distance, 2);
}
END_TEST


START_TEST(test_mmr_map_has_entry_happy) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    mmr_map_insert_entry(&map, "hello", "world");
    mmr_map_insert_entry(&map, "another_key", "42");
    mmr_map_insert_entry(&map, "null", NULL);

    uint8_t result;

    result = (uint8_t)mmr_map_has_entry(&map, "hello");
    ck_assert_uint_eq(result, 1);

    result = (uint8_t)mmr_map_has_entry(&map, "another_key");
    ck_assert_uint_eq(result, 1);

    result = (uint8_t)mmr_map_has_entry(&map, "null");
    ck_assert_uint_eq(result, 1);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_mmr_map_has_entry_unhappy) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    uint8_t result = (uint8_t)mmr_map_has_entry(&map, "unknown");
    ck_assert_uint_eq(result, 0);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_mmr_map_get_entry_happy) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);

    mmr_map_insert_entry(&map, "hello", "world");

    char* result = mmr_map_get_entry(&map, "hello");
    ck_assert_str_eq(result, "world");

    uint8_t* answer = malloc(sizeof(uint8_t));
    *answer = 42;

    mmr_map_insert_entry(&map, "answer", answer);

    uint8_t* result_uint = mmr_map_get_entry(&map, "answer");
    ck_assert_uint_eq(*result_uint, 42);
    free(answer);

    mmr_map_insert_entry(&map, "null", NULL);
    void* result_null = mmr_map_get_entry(&map, "null");
    ck_assert_ptr_null(result_null);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_mmr_map_get_entry_unhappy) {
    mmr_map_t map = {
        .count = 0,
        .capacity = 10,
        .entries = NULL
    };


    mmr_map_alloc(&map);
    ck_assert_ptr_nonnull(map.entries);
    mmr_map_insert_entry(&map, "hello", "world");
    mmr_map_insert_entry(&map, "another", "entry");

    char* result = mmr_map_get_entry(&map, "unknown");
    ck_assert_ptr_null(result);

    mmr_map_free(&map);
}
END_TEST


START_TEST(test_murmurhash_hashing) {
    char* test_cases[7] = {
        "a",
        "Hello",
        "world",
        "MurmurHash3",
        "big random string",
        "1234567890",
        "int main(int argc, char** argv)"
    };

    uint32_t results[7] = {
        1009084850,
        316307400,
        4220927227,
        1473193682,
        162831409,
        839148365,
        2389356776
    };

    for (size_t i = 0; i < 2; i++) {
        char* key = test_cases[i];
        uint32_t hash = murmurhash3(key, strlen(key), 0);
        ck_assert_int_eq(hash, results[i]);
    }
}
END_TEST


int
main(void) {
    Suite *suite = suite_create("Hash map suite");

    TCase *tc = tcase_create("Hash map");
    tcase_add_test(tc, test_mmr_map_create);
    tcase_add_test(tc, test_mmr_map_free);
    tcase_add_test(tc, test_mmr_map_has_entry_happy);
    tcase_add_test(tc, test_mmr_map_has_entry_unhappy);
    tcase_add_test(tc, test_mmr_map_get_entry_happy);
    tcase_add_test(tc, test_mmr_map_get_entry_unhappy);
    tcase_add_test(tc, test_mmr_map_insert_entry_upates_count);
    tcase_add_test(tc, test_mmr_map_insert_entry_adds_an_entry);
    tcase_add_test(tc, test_mmr_map_resize);
    tcase_add_test(tc, test_mmr_map_autoresize);
    tcase_add_test(tc, test_robin_hood_insert_probe_distance);

    tcase_add_test(tc, test_murmurhash_hashing);

    suite_add_tcase(suite, tc);

    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    int num_failed = srunner_ntests_failed(runner);

    srunner_free(runner);
    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
