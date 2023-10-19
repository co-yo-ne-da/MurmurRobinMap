#include <stdlib.h>
#include <check.h>
#include <hashmap.h>

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


START_TEST(test_hash_map_has_entry_happy) {
    hash_map_t* map = hash_map_create(10);
    ck_assert_ptr_nonnull(map);

    hash_map_insert_entry(map, "hello", "world");

    uint8_t result = (uint8_t)hash_map_has_entry(map, "hello");
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


int
main(void) {
    Suite *suite = suite_create("Hash map suite");

    TCase *tc = tcase_create("Hash map");
    tcase_add_test(tc, test_hash_map_create);
    tcase_add_test(tc, test_hash_map_free);
    tcase_add_test(tc, test_hash_map_has_entry_happy);
    tcase_add_test(tc, test_hash_map_has_entry_unhappy);
    suite_add_tcase(suite, tc);

    SRunner *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    int num_failed = srunner_ntests_failed(runner);

    srunner_free(runner);
    return (num_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
