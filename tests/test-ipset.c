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

const char  *template = "/tmp/test-ipset-XXXXXX";
const char  *base_filename = "/test.set";

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

START_TEST(test_set_starts_empty)
{
    ip_set_t  set;

    ipset_init(&set);
    fail_unless(ipset_is_empty(&set),
                "Set should start empty");
    ipset_done(&set);
}
END_TEST

START_TEST(test_empty_sets_equal)
{
    ip_set_t  set1, set2;

    ipset_init(&set1);
    ipset_init(&set2);
    fail_unless(ipset_is_equal(&set1, &set2),
                "Empty sets should be equal");
    ipset_done(&set1);
    ipset_done(&set2);
}
END_TEST

START_TEST(test_empty_sets_not_unequal)
{
    ip_set_t  set1, set2;

    ipset_init(&set1);
    ipset_init(&set2);
    fail_if(ipset_is_not_equal(&set1, &set2),
            "Empty sets should not be unequal");
    ipset_done(&set1);
    ipset_done(&set2);
}
END_TEST

START_TEST(test_store_empty)
{
    FILE  *file;
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);

    file = fopen(full_filename, "w");
    fail_unless(ipset_save(&set, file),
                "Could not save set to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_set = ipset_load(file);
    fail_if(read_set == NULL,
            "Could not read set from disk");
    fclose(file);

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST


/*-----------------------------------------------------------------------
 * IPv4 tests
 */

START_TEST(test_ipv4_insert)
{
    ip_set_t  set;

    ipset_init(&set);

    fail_if(ipset_ipv4_add(&set, &IPV4_ADDR_1),
            "Element should not be present");

    fail_unless(ipset_ipv4_add(&set, &IPV4_ADDR_1),
                "Element should be present");

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv4_insert_network)
{
    ip_set_t  set;

    ipset_init(&set);

    fail_if(ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24),
            "Element should not be present");

    fail_unless(ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24),
                "Element should be present");

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv4_bad_netmask_01)
{
    ip_set_t  set;

    ipset_init(&set);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 0);
    fail_unless(ipset_is_empty(&set),
                "Bad netmask shouldn't change set");
    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv4_bad_netmask_02)
{
    ip_set_t  set;

    ipset_init(&set);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 33);
    fail_unless(ipset_is_empty(&set),
                "Bad netmask shouldn't change set");
    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv4_equality_1)
{
    ip_set_t  set1, set2;

    ipset_init(&set1);
    ipset_ipv4_add(&set1, &IPV4_ADDR_1);

    ipset_init(&set2);
    ipset_ipv4_add(&set2, &IPV4_ADDR_1);

    fail_unless(ipset_is_equal(&set1, &set2),
                "Expected {x} == {x}");

    ipset_done(&set1);
    ipset_done(&set2);
}
END_TEST

START_TEST(test_ipv4_inequality_1)
{
    ip_set_t  set1, set2;

    ipset_init(&set1);
    ipset_ipv4_add(&set1, &IPV4_ADDR_1);

    ipset_init(&set2);
    ipset_ipv4_add_network(&set2, &IPV4_ADDR_1, 24);

    fail_unless(ipset_is_not_equal(&set1, &set2),
                "Expected {x} != {x}");

    ipset_done(&set1);
    ipset_done(&set2);
}
END_TEST

START_TEST(test_ipv4_memory_size_1)
{
    ip_set_t  set;
    size_t  expected, actual;

    ipset_init(&set);
    ipset_ipv4_add(&set, &IPV4_ADDR_1);

#if SIZE_MAX == UINT32_MAX
    expected = 528;
#else
    expected = 1056;
#endif
    actual = ipset_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv4_memory_size_2)
{
    ip_set_t  set;
    size_t  expected, actual;

    ipset_init(&set);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24);

#if SIZE_MAX == UINT32_MAX
    expected = 400;
#else
    expected = 800;
#endif
    actual = ipset_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv4_store_01)
{
    FILE  *file;
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv4_add(&set, &IPV4_ADDR_1);

    file = fopen(full_filename, "w");
    fail_unless(ipset_save(&set, file),
                "Could not save set to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_set = ipset_load(file);
    fail_if(read_set == NULL,
            "Could not read set from disk");
    fclose(file);

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv4_store_02)
{
    FILE  *file;
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24);

    file = fopen(full_filename, "w");
    fail_unless(ipset_save(&set, file),
                "Could not save set to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_set = ipset_load(file);
    fail_if(read_set == NULL,
            "Could not read set from disk");
    fclose(file);

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv4_store_03)
{
    FILE  *file;
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv4_add(&set, &IPV4_ADDR_1);
    ipset_ipv4_add(&set, &IPV4_ADDR_2);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_3, 24);

    file = fopen(full_filename, "w");
    fail_unless(ipset_save(&set, file),
                "Could not save set to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_set = ipset_load(file);
    fail_if(read_set == NULL,
            "Could not read set from disk");
    fclose(file);

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST


/*-----------------------------------------------------------------------
 * IPv6 tests
 */

START_TEST(test_ipv6_insert)
{
    ip_set_t  set;

    ipset_init(&set);

    fail_if(ipset_ipv6_add(&set, &IPV6_ADDR_1),
            "Element should not be present");

    fail_unless(ipset_ipv6_add(&set, &IPV6_ADDR_1),
                "Element should be present");

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv6_insert_network)
{
    ip_set_t  set;

    ipset_init(&set);

    fail_if(ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 32),
            "Element should not be present");

    fail_unless(ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 32),
                "Element should be present");

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv6_bad_netmask_01)
{
    ip_set_t  set;

    ipset_init(&set);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 0);
    fail_unless(ipset_is_empty(&set),
                "Bad netmask shouldn't change set");
    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv6_bad_netmask_02)
{
    ip_set_t  set;

    ipset_init(&set);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 129);
    fail_unless(ipset_is_empty(&set),
                "Bad netmask shouldn't change set");
    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv6_equality_1)
{
    ip_set_t  set1, set2;

    ipset_init(&set1);
    ipset_ipv6_add(&set1, &IPV6_ADDR_1);

    ipset_init(&set2);
    ipset_ipv6_add(&set2, &IPV6_ADDR_1);

    fail_unless(ipset_is_equal(&set1, &set2),
                "Expected {x} == {x}");

    ipset_done(&set1);
    ipset_done(&set2);
}
END_TEST

START_TEST(test_ipv6_inequality_1)
{
    ip_set_t  set1, set2;

    ipset_init(&set1);
    ipset_ipv6_add(&set1, &IPV6_ADDR_1);

    ipset_init(&set2);
    ipset_ipv6_add_network(&set2, &IPV6_ADDR_1, 32);

    fail_unless(ipset_is_not_equal(&set1, &set2),
                "Expected {x} != {x}");

    ipset_done(&set1);
    ipset_done(&set2);
}
END_TEST

START_TEST(test_ipv6_memory_size_1)
{
    ip_set_t  set;
    size_t  expected, actual;

    ipset_init(&set);
    ipset_ipv6_add(&set, &IPV6_ADDR_1);

#if SIZE_MAX == UINT32_MAX
    expected = 2064;
#else
    expected = 4128;
#endif
    actual = ipset_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv6_memory_size_2)
{
    ip_set_t  set;
    size_t  expected, actual;

    ipset_init(&set);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 24);

#if SIZE_MAX == UINT32_MAX
    expected = 400;
#else
    expected = 800;
#endif
    actual = ipset_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_done(&set);
}
END_TEST

START_TEST(test_ipv6_store_01)
{
    FILE  *file;
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv6_add(&set, &IPV6_ADDR_1);

    file = fopen(full_filename, "w");
    fail_unless(ipset_save(&set, file),
                "Could not save set to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_set = ipset_load(file);
    fail_if(read_set == NULL,
            "Could not read set from disk");
    fclose(file);

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv6_store_02)
{
    FILE  *file;
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 24);

    file = fopen(full_filename, "w");
    fail_unless(ipset_save(&set, file),
                "Could not save set to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_set = ipset_load(file);
    fail_if(read_set == NULL,
            "Could not read set from disk");
    fclose(file);

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv6_store_03)
{
    FILE  *file;
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv6_add(&set, &IPV6_ADDR_1);
    ipset_ipv6_add(&set, &IPV6_ADDR_2);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_3, 24);

    file = fopen(full_filename, "w");
    fail_unless(ipset_save(&set, file),
                "Could not save set to disk");
    fclose(file);

    file = fopen(full_filename, "r");
    read_set = ipset_load(file);
    fail_if(read_set == NULL,
            "Could not read set from disk");
    fclose(file);

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
ipset_suite()
{
    Suite  *s = suite_create("ipset");

    TCase  *tc_general = tcase_create("general");
    tcase_add_checked_fixture(tc_general, NULL, remove_file);
    tcase_add_test(tc_general, test_set_starts_empty);
    tcase_add_test(tc_general, test_empty_sets_equal);
    tcase_add_test(tc_general, test_empty_sets_not_unequal);
    tcase_add_test(tc_general, test_store_empty);
    suite_add_tcase(s, tc_general);

    TCase  *tc_ipv4 = tcase_create("ipv4");
    tcase_add_checked_fixture(tc_ipv4, NULL, remove_file);
    tcase_add_test(tc_ipv4, test_ipv4_insert);
    tcase_add_test(tc_ipv4, test_ipv4_insert_network);
    tcase_add_test(tc_ipv4, test_ipv4_bad_netmask_01);
    tcase_add_test(tc_ipv4, test_ipv4_bad_netmask_02);
    tcase_add_test(tc_ipv4, test_ipv4_equality_1);
    tcase_add_test(tc_ipv4, test_ipv4_inequality_1);
    tcase_add_test(tc_ipv4, test_ipv4_memory_size_1);
    tcase_add_test(tc_ipv4, test_ipv4_memory_size_2);
    tcase_add_test(tc_ipv4, test_ipv4_store_01);
    tcase_add_test(tc_ipv4, test_ipv4_store_02);
    tcase_add_test(tc_ipv4, test_ipv4_store_03);
    suite_add_tcase(s, tc_ipv4);

    TCase  *tc_ipv6 = tcase_create("ipv6");
    tcase_add_checked_fixture(tc_ipv6, NULL, remove_file);
    tcase_add_test(tc_ipv6, test_ipv6_insert);
    tcase_add_test(tc_ipv6, test_ipv6_insert_network);
    tcase_add_test(tc_ipv6, test_ipv6_bad_netmask_01);
    tcase_add_test(tc_ipv6, test_ipv6_bad_netmask_02);
    tcase_add_test(tc_ipv6, test_ipv6_equality_1);
    tcase_add_test(tc_ipv6, test_ipv6_inequality_1);
    tcase_add_test(tc_ipv6, test_ipv6_memory_size_1);
    tcase_add_test(tc_ipv6, test_ipv6_memory_size_2);
    tcase_add_test(tc_ipv6, test_ipv6_store_01);
    tcase_add_test(tc_ipv6, test_ipv6_store_02);
    tcase_add_test(tc_ipv6, test_ipv6_store_03);
    suite_add_tcase(s, tc_ipv6);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = ipset_suite();
    SRunner  *runner = srunner_create(suite);

    ipset_init_library();
    create_tempdir();

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    remove_tempdir();

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
