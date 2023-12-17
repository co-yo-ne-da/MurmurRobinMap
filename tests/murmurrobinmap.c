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


START_TEST(test_hash_map_free) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);

    hash_map_free(map);
    ck_assert_ptr_null(map->entries);
}
END_TEST


START_TEST(test_hash_map_create) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);
    ck_assert_ptr_nonnull(map->entries);
    ck_assert_uint_eq(map->capacity, 10);
    ck_assert_uint_eq(map->count, 0);

    hash_map_free(map);
}
END_TEST


START_TEST(test_hash_map_insert_entry_upates_count) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);
    ck_assert_uint_eq(map->count, 0);

    hash_map_insert_entry(map, "hello", "world");
    ck_assert_uint_eq(map->count, 1);

    hash_map_insert_entry(map, "another", "entry");
    ck_assert_uint_eq(map->count, 2);

    hash_map_free(map);
}
END_TEST


START_TEST(test_hash_map_insert_entry_adds_an_entry) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);
    ck_assert_uint_eq(map->count, 0);

    hash_map_insert_entry(map, "hello", "world");
    
    uint32_t index = hash_map_find_index(map, "hello");
    entry_t* entry = map->entries[index];
    ck_assert_ptr_nonnull(entry);
    ck_assert_str_eq((char*)entry->value, "world");

    hash_map_free(map);
}
END_TEST


START_TEST(test_hash_map_resize) {
    uint32_t capacity = 10;
    hash_map_t* map = hash_map_create(capacity);
    ck_assert_ptr_nonnull(map);
    ck_assert_uint_eq(map->capacity, capacity);

    hash_map_resize(map);

    ck_assert_uint_eq(map->capacity, capacity * GROW_FACTOR);

    hash_map_free(map);
}
END_TEST


START_TEST(test_robin_hood_insert_probe_distance) {
    uint32_t capacity = 10;
    hash_map_t* map = hash_map_create(capacity);
    ck_assert_ptr_nonnull(map);

    entry_t* entry = create_entry("test11", "hello");
    entry_t* test_entry = create_entry("test12", "world");
    entry_t* test_entry2 = create_entry("test16", "test");

    robin_hood_insert(map, entry);
    uint32_t index = hash_map_find_index(map, "test11");
    ck_assert_str_eq((char*)map->entries[index]->value, "hello");
    ck_assert_uint_eq(map->entries[index]->probe_distance, 0);

    robin_hood_insert(map, test_entry);
    ck_assert_str_eq((char*)map->entries[(index + 1) % capacity]->value, "world");
    ck_assert_uint_eq(map->entries[(index + 1) % capacity]->probe_distance, 1);

    robin_hood_insert(map, test_entry2);
    ck_assert_str_eq((char*)map->entries[(index + 2) % capacity]->value, "test");
    ck_assert_uint_eq(map->entries[(index + 2) % capacity]->probe_distance, 2);
}
END_TEST


START_TEST(test_hash_map_has_entry_happy) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);

    hash_map_insert_entry(map, "hello", "world");
    hash_map_insert_entry(map, "another_key", "42");
    hash_map_insert_entry(map, "null", NULL);

    uint8_t result;

    result = (uint8_t)hash_map_has_entry(map, "hello");
    ck_assert_uint_eq(result, 1);

    result = (uint8_t)hash_map_has_entry(map, "another_key");
    ck_assert_uint_eq(result, 1);

    result = (uint8_t)hash_map_has_entry(map, "null");
    ck_assert_uint_eq(result, 1);

    hash_map_free(map);
}
END_TEST


START_TEST(test_hash_map_has_entry_unhappy) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);

    uint8_t result = (uint8_t)hash_map_has_entry(map, "unknown");
    ck_assert_uint_eq(result, 0);

    hash_map_free(map);
}
END_TEST


START_TEST(test_hash_map_get_entry_happy) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);

    hash_map_insert_entry(map, "hello", "world");

    char* result = hash_map_get_entry(map, "hello");
    ck_assert_str_eq(result, "world");

    uint8_t* answer = malloc(sizeof(uint8_t));
    *answer = 42;

    hash_map_insert_entry(map, "answer", answer);

    uint8_t* result_uint = hash_map_get_entry(map, "answer");
    ck_assert_uint_eq(*result_uint, 42);
    free(answer);

    hash_map_insert_entry(map, "null", NULL);
    void* result_null = hash_map_get_entry(map, "null");
    ck_assert_ptr_null(result_null);

    hash_map_free(map);
}
END_TEST


START_TEST(test_hash_map_get_entry_unhappy) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);

    hash_map_insert_entry(map, "hello", "world");
    hash_map_insert_entry(map, "another", "entry");

    char* result = hash_map_get_entry(map, "unknown");
    ck_assert_ptr_null(result);

    hash_map_free(map);
}
END_TEST


START_TEST(test_murmurhash_hashing) {
    char* test_cases[2] = {
        "Hello",
        "world"
    };

    uint32_t results[2] = {
        316307400,
        4220927227
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
    tcase_add_test(tc, test_hash_map_create);
    tcase_add_test(tc, test_hash_map_free);
    tcase_add_test(tc, test_hash_map_has_entry_happy);
    tcase_add_test(tc, test_hash_map_has_entry_unhappy);
    tcase_add_test(tc, test_hash_map_get_entry_happy);
    tcase_add_test(tc, test_hash_map_get_entry_unhappy);
    tcase_add_test(tc, test_hash_map_insert_entry_upates_count);
    tcase_add_test(tc, test_hash_map_insert_entry_adds_an_entry);
    tcase_add_test(tc, test_hash_map_resize);
    tcase_add_test(tc, test_robin_hood_insert_probe_distance);

    tcase_add_test(tc, test_murmurhash_hashing);

    suite_add_tcase(suite, tc);

    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    int num_failed = srunner_ntests_failed(runner);

    srunner_free(runner);
    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
