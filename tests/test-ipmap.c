/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <check.h>

#include <ipset/ipset.h>


/*-----------------------------------------------------------------------
 * Sample IP addresses
 */

typedef uint8_t  ipv4_addr_t[4];
typedef uint8_t  ipv6_addr_t[16];

static ipv4_addr_t  IPV4_ADDR_1 = "\xc0\xa8\x01\x64"; /* 192.168.1.100 */
static ipv4_addr_t  IPV4_ADDR_2 = "\xc0\xa8\x01\x65"; /* 192.168.1.101 */
static ipv4_addr_t  IPV4_ADDR_3 = "\xc0\xa8\x02\x64"; /* 192.168.2.100 */

static ipv6_addr_t  IPV6_ADDR_1 =
"\xfe\x80\x00\x00\x00\x00\x00\x00\x02\x1e\xc2\xff\xfe\x9f\xe8\xe1";
static ipv6_addr_t  IPV6_ADDR_2 =
"\xfe\x80\x00\x00\x00\x00\x00\x00\x02\x1e\xc2\xff\xfe\x9f\xe8\xe2";
static ipv6_addr_t  IPV6_ADDR_3 =
"\xfe\x80\x00\x01\x00\x00\x00\x00\x02\x1e\xc2\xff\xfe\x9f\xe8\xe1";


/*-----------------------------------------------------------------------
 * Helper functions
 */

const char  *template = "/tmp/test-ipmap-XXXXXX";
const char  *base_filename = "/test.map";

char  *tempdir;
char  *full_filename;


static void
create_tempdir()
{
    size_t  dir_len;
    size_t  file_len;
    size_t  len;

    tempdir = strdup(template);
    tempdir = mkdtemp(tempdir);

    dir_len = strlen(tempdir);
    file_len = strlen(base_filename);
    len = dir_len + file_len + 1;

    full_filename = (char *) malloc(len);
    strncpy(full_filename, tempdir, dir_len);
    strncpy(full_filename + dir_len, base_filename, file_len + 1);
}


static void
remove_tempdir()
{
    if (rmdir(tempdir) != 0)
    {
        if (errno == ENOTEMPTY)
        {
            fprintf(stderr,
                    "Some test case didn't clean "
                    "up after itself.\n");
        }
    }

    free(tempdir);
    free(full_filename);
}


static void
remove_file()
{
    /*
     * All of the tests that create an output file call it the same
     * thing.  This function is registered as a cleanup function for
     * each test case, and deletes this if it exists.
     */

    struct stat  sb;

    if (stat(full_filename, &sb) == 0)
    {
        //printf("Removing %s...\n", full_filename);
        unlink(full_filename);
    }
}


/*-----------------------------------------------------------------------
 * General tests
 */

START_TEST(test_map_starts_empty)
{
    ip_map_t  map;

    ipmap_init(&map, 0);
    fail_unless(ipmap_is_empty(&map),
                "Map should start empty");
    ipmap_done(&map);
}
END_TEST

START_TEST(test_empty_maps_equal)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_init(&map2, 0);
    fail_unless(ipmap_is_equal(&map1, &map2),
                "Empty maps should be equal");
    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_empty_maps_not_unequal)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_init(&map2, 0);
    fail_if(ipmap_is_not_equal(&map1, &map2),
            "Empty maps should not be unequal");
    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_different_defaults_unequal)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_init(&map2, 1);
    fail_if(ipmap_is_equal(&map1, &map2),
            "Empty maps with different defaults "
            "should be unequal");
    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_store_empty_01)
{
    FILE  *file;
    ip_map_t  map;
    ip_map_t  *read_map;

    ipmap_init(&map, 0);

    file = fopen(full_filename, "w");
    fail_unless(ipmap_save(&map, file),
                "Could not save map to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_map = ipmap_load(file);
    fail_if(read_map == NULL,
            "Could not read map from disk");
    fclose(file);

    fail_unless(ipmap_is_equal(&map, read_map),
                "Map not same after saving/loading");

    ipmap_done(&map);
    ipmap_free(read_map);
}
END_TEST

START_TEST(test_store_empty_02)
{
    FILE  *file;
    ip_map_t  map;
    ip_map_t  *read_map;

    ipmap_init(&map, 1);

    file = fopen(full_filename, "w");
    fail_unless(ipmap_save(&map, file),
                "Could not save map to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_map = ipmap_load(file);
    fail_if(read_map == NULL,
            "Could not read map from disk");
    fclose(file);

    fail_unless(ipmap_is_equal(&map, read_map),
                "Map not same after saving/loading");

    ipmap_done(&map);
    ipmap_free(read_map);
}
END_TEST


/*-----------------------------------------------------------------------
 * IPv4 tests
 */

START_TEST(test_ipv4_insert_01)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv4_set(&map, &IPV4_ADDR_1, 1);

    fail_unless(ipmap_ipv4_get(&map, &IPV4_ADDR_1) == 1,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_insert_02)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv4_set(&map, &IPV4_ADDR_1, 1);

    fail_unless(ipmap_ipv4_get(&map, &IPV4_ADDR_2) == 0,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_insert_03)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv4_set(&map, &IPV4_ADDR_1, 1);

    fail_unless(ipmap_ipv4_get(&map, &IPV4_ADDR_3) == 0,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_insert_network_01)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv4_set_network(&map, &IPV4_ADDR_1, 24, 1);

    fail_unless(ipmap_ipv4_get(&map, &IPV4_ADDR_1) == 1,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_insert_network_02)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv4_set_network(&map, &IPV4_ADDR_1, 24, 1);

    fail_unless(ipmap_ipv4_get(&map, &IPV4_ADDR_2) == 1,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_insert_network_03)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv4_set_network(&map, &IPV4_ADDR_1, 24, 1);

    fail_unless(ipmap_ipv4_get(&map, &IPV4_ADDR_3) == 0,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_bad_netmask_01)
{
    ip_map_t  map;

    ipmap_init(&map, 0);
    ipmap_ipv4_set_network(&map, &IPV4_ADDR_1, 0, 1);
    fail_unless(ipmap_is_empty(&map),
                "Bad netmask shouldn't change map");
    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_bad_netmask_02)
{
    ip_map_t  map;

    ipmap_init(&map, 0);
    ipmap_ipv4_set_network(&map, &IPV4_ADDR_1, 33, 1);
    fail_unless(ipmap_is_empty(&map),
                "Bad netmask shouldn't change map");
    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_equality_1)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_ipv4_set(&map1, &IPV4_ADDR_1, 1);

    ipmap_init(&map2, 0);
    ipmap_ipv4_set(&map2, &IPV4_ADDR_1, 1);

    fail_unless(ipmap_is_equal(&map1, &map2),
                "Expected {x} == {x}");

    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_ipv4_inequality_1)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_ipv4_set(&map1, &IPV4_ADDR_1, 1);

    ipmap_init(&map2, 0);
    ipmap_ipv4_set_network(&map2, &IPV4_ADDR_1, 24, 1);

    fail_unless(ipmap_is_not_equal(&map1, &map2),
                "Expected {x} != {x}");

    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_ipv4_inequality_2)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_ipv4_set(&map1, &IPV4_ADDR_1, 1);

    ipmap_init(&map2, 0);
    ipmap_ipv4_set(&map2, &IPV4_ADDR_1, 2);

    fail_unless(ipmap_is_not_equal(&map1, &map2),
                "Expected {x} != {x}");

    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_ipv4_memory_size_1)
{
    ip_map_t  map;
    size_t  expected, actual;

    ipmap_init(&map, 0);
    ipmap_ipv4_set(&map, &IPV4_ADDR_1, 1);

    expected = 1088;
    actual = ipmap_memory_size(&map);

    fail_unless(expected == actual,
                "Expected map to be %zu bytes, got %zu bytes",
                expected, actual);

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_memory_size_2)
{
    ip_map_t  map;
    size_t  expected, actual;

    ipmap_init(&map, 0);
    ipmap_ipv4_set_network(&map, &IPV4_ADDR_1, 24, 1);

    expected = 832;
    actual = ipmap_memory_size(&map);

    fail_unless(expected == actual,
                "Expected map to be %zu bytes, got %zu bytes",
                expected, actual);

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv4_store_01)
{
    FILE  *file;
    ip_map_t  map;
    ip_map_t  *read_map;

    ipmap_init(&map, 0);
    ipmap_ipv4_set(&map, &IPV4_ADDR_1, 1);
    ipmap_ipv4_set(&map, &IPV4_ADDR_2, 2);
    ipmap_ipv4_set_network(&map, &IPV4_ADDR_3, 24, 2);

    file = fopen(full_filename, "w");
    fail_unless(ipmap_save(&map, file),
                "Could not save map to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_map = ipmap_load(file);
    fail_if(read_map == NULL,
            "Could not read map from disk");
    fclose(file);

    fail_unless(ipmap_is_equal(&map, read_map),
                "Map not same after saving/loading");

    ipmap_done(&map);
    ipmap_free(read_map);
}
END_TEST


/*-----------------------------------------------------------------------
 * IPv6 tests
 */

START_TEST(test_ipv6_insert_01)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv6_set(&map, &IPV6_ADDR_1, 1);

    fail_unless(ipmap_ipv6_get(&map, &IPV6_ADDR_1) == 1,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_insert_02)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv6_set(&map, &IPV6_ADDR_1, 1);

    fail_unless(ipmap_ipv6_get(&map, &IPV6_ADDR_2) == 0,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_insert_03)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv6_set(&map, &IPV6_ADDR_1, 1);

    fail_unless(ipmap_ipv6_get(&map, &IPV6_ADDR_3) == 0,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_insert_network_01)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv6_set_network(&map, &IPV6_ADDR_1, 32, 1);

    fail_unless(ipmap_ipv6_get(&map, &IPV6_ADDR_1) == 1,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_insert_network_02)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv6_set_network(&map, &IPV6_ADDR_1, 32, 1);

    fail_unless(ipmap_ipv6_get(&map, &IPV6_ADDR_2) == 1,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_insert_network_03)
{
    ip_map_t  map;

    ipmap_init(&map, 0);

    ipmap_ipv6_set_network(&map, &IPV6_ADDR_1, 32, 1);

    fail_unless(ipmap_ipv6_get(&map, &IPV6_ADDR_3) == 0,
                "Element should be present");

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_bad_netmask_01)
{
    ip_map_t  map;

    ipmap_init(&map, 0);
    ipmap_ipv6_set_network(&map, &IPV6_ADDR_1, 0, 1);
    fail_unless(ipmap_is_empty(&map),
                "Bad netmask shouldn't change map");
    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_bad_netmask_02)
{
    ip_map_t  map;

    ipmap_init(&map, 0);
    ipmap_ipv6_set_network(&map, &IPV6_ADDR_1, 129, 1);
    fail_unless(ipmap_is_empty(&map),
                "Bad netmask shouldn't change map");
    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_equality_1)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_ipv6_set(&map1, &IPV6_ADDR_1, 1);

    ipmap_init(&map2, 0);
    ipmap_ipv6_set(&map2, &IPV6_ADDR_1, 1);

    fail_unless(ipmap_is_equal(&map1, &map2),
                "Expected {x} == {x}");

    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_ipv6_inequality_1)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_ipv6_set(&map1, &IPV6_ADDR_1, 1);

    ipmap_init(&map2, 0);
    ipmap_ipv6_set_network(&map2, &IPV6_ADDR_1, 32, 1);

    fail_unless(ipmap_is_not_equal(&map1, &map2),
                "Expected {x} != {x}");

    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_ipv6_inequality_2)
{
    ip_map_t  map1, map2;

    ipmap_init(&map1, 0);
    ipmap_ipv6_set(&map1, &IPV6_ADDR_1, 1);

    ipmap_init(&map2, 0);
    ipmap_ipv6_set(&map2, &IPV6_ADDR_1, 2);

    fail_unless(ipmap_is_not_equal(&map1, &map2),
                "Expected {x} != {x}");

    ipmap_done(&map1);
    ipmap_done(&map2);
}
END_TEST

START_TEST(test_ipv6_memory_size_1)
{
    ip_map_t  map;
    size_t  expected, actual;

    ipmap_init(&map, 0);
    ipmap_ipv6_set(&map, &IPV6_ADDR_1, 1);

    expected = 4160;
    actual = ipmap_memory_size(&map);

    fail_unless(expected == actual,
                "Expected map to be %zu bytes, got %zu bytes",
                expected, actual);

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_memory_size_2)
{
    ip_map_t  map;
    size_t  expected, actual;

    ipmap_init(&map, 0);
    ipmap_ipv6_set_network(&map, &IPV6_ADDR_1, 32, 1);

    expected = 1088;
    actual = ipmap_memory_size(&map);

    fail_unless(expected == actual,
                "Expected map to be %zu bytes, got %zu bytes",
                expected, actual);

    ipmap_done(&map);
}
END_TEST

START_TEST(test_ipv6_store_01)
{
    FILE  *file;
    ip_map_t  map;
    ip_map_t  *read_map;

    ipmap_init(&map, 0);
    ipmap_ipv6_set(&map, &IPV6_ADDR_1, 1);
    ipmap_ipv6_set(&map, &IPV6_ADDR_2, 2);
    ipmap_ipv6_set_network(&map, &IPV6_ADDR_3, 32, 2);

    file = fopen(full_filename, "w");
    fail_unless(ipmap_save(&map, file),
                "Could not save map to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_map = ipmap_load(file);
    fail_if(read_map == NULL,
            "Could not read map from disk");
    fclose(file);

    fail_unless(ipmap_is_equal(&map, read_map),
                "Map not same after saving/loading");

    ipmap_done(&map);
    ipmap_free(read_map);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
ipmap_suite()
{
    Suite  *s = suite_create("ipmap");

    TCase  *tc_general = tcase_create("general");
    tcase_add_checked_fixture(tc_general, NULL, remove_file);
    tcase_add_test(tc_general, test_map_starts_empty);
    tcase_add_test(tc_general, test_empty_maps_equal);
    tcase_add_test(tc_general, test_empty_maps_not_unequal);
    tcase_add_test(tc_general, test_different_defaults_unequal);
    tcase_add_test(tc_general, test_store_empty_01);
    tcase_add_test(tc_general, test_store_empty_02);
    suite_add_tcase(s, tc_general);

    TCase  *tc_ipv4 = tcase_create("ipv4");
    tcase_add_checked_fixture(tc_ipv4, NULL, remove_file);
    tcase_add_test(tc_ipv4, test_ipv4_insert_01);
    tcase_add_test(tc_ipv4, test_ipv4_insert_02);
    tcase_add_test(tc_ipv4, test_ipv4_insert_03);
    tcase_add_test(tc_ipv4, test_ipv4_insert_network_01);
    tcase_add_test(tc_ipv4, test_ipv4_insert_network_02);
    tcase_add_test(tc_ipv4, test_ipv4_insert_network_03);
    tcase_add_test(tc_ipv4, test_ipv4_bad_netmask_01);
    tcase_add_test(tc_ipv4, test_ipv4_bad_netmask_02);
    tcase_add_test(tc_ipv4, test_ipv4_equality_1);
    tcase_add_test(tc_ipv4, test_ipv4_inequality_1);
    tcase_add_test(tc_ipv4, test_ipv4_inequality_2);
    tcase_add_test(tc_ipv4, test_ipv4_memory_size_1);
    tcase_add_test(tc_ipv4, test_ipv4_memory_size_2);
    tcase_add_test(tc_ipv4, test_ipv4_store_01);
    suite_add_tcase(s, tc_ipv4);

    TCase  *tc_ipv6 = tcase_create("ipv6");
    tcase_add_checked_fixture(tc_ipv6, NULL, remove_file);
    tcase_add_test(tc_ipv6, test_ipv6_insert_01);
    tcase_add_test(tc_ipv6, test_ipv6_insert_02);
    tcase_add_test(tc_ipv6, test_ipv6_insert_03);
    tcase_add_test(tc_ipv6, test_ipv6_insert_network_01);
    tcase_add_test(tc_ipv6, test_ipv6_insert_network_02);
    tcase_add_test(tc_ipv6, test_ipv6_insert_network_03);
    tcase_add_test(tc_ipv6, test_ipv6_bad_netmask_01);
    tcase_add_test(tc_ipv6, test_ipv6_bad_netmask_02);
    tcase_add_test(tc_ipv6, test_ipv6_equality_1);
    tcase_add_test(tc_ipv6, test_ipv6_inequality_1);
    tcase_add_test(tc_ipv6, test_ipv6_inequality_2);
    tcase_add_test(tc_ipv6, test_ipv6_memory_size_1);
    tcase_add_test(tc_ipv6, test_ipv6_memory_size_2);
    tcase_add_test(tc_ipv6, test_ipv6_store_01);
    suite_add_tcase(s, tc_ipv6);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = ipmap_suite();
    SRunner  *runner = srunner_create(suite);

    ipset_init_library();
    create_tempdir();

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    remove_tempdir();

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
