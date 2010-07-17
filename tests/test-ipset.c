/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

#include <check.h>
#include <glib.h>
#include <gio/gio.h>

#include <ipset/ipset.h>


/*-----------------------------------------------------------------------
 * Sample IP addresses
 */

typedef guint8  ipv4_addr_t[4];
typedef guint8  ipv6_addr_t[16];

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
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);

    GOutputStream  *ostream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mostream =
        G_MEMORY_OUTPUT_STREAM(ostream);

    fail_unless(ipset_save(ostream, &set, NULL),
                "Could not save set");

    GInputStream  *istream =
        g_memory_input_stream_new_from_data
        (g_memory_output_stream_get_data(mostream),
         g_memory_output_stream_get_data_size(mostream),
         NULL);

    read_set = ipset_load(istream, NULL);
    fail_if(read_set == NULL,
            "Could not read set");

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    g_object_unref(ostream);
    g_object_unref(istream);
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

#if GLIB_SIZEOF_VOID_P == 4
    expected = 396;
#elif GLIB_SIZEOF_VOID_P == 8
    expected = 792;
#else
#   error "Unknown architecture: not 32-bit or 64-bit"
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

#if GLIB_SIZEOF_VOID_P == 4
    expected = 300;
#elif GLIB_SIZEOF_VOID_P == 8
    expected = 600;
#else
#   error "Unknown architecture: not 32-bit or 64-bit"
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
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv4_add(&set, &IPV4_ADDR_1);

    GOutputStream  *ostream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mostream =
        G_MEMORY_OUTPUT_STREAM(ostream);

    fail_unless(ipset_save(ostream, &set, NULL),
                "Could not save set");

    GInputStream  *istream =
        g_memory_input_stream_new_from_data
        (g_memory_output_stream_get_data(mostream),
         g_memory_output_stream_get_data_size(mostream),
         NULL);

    read_set = ipset_load(istream, NULL);
    fail_if(read_set == NULL,
            "Could not read set");

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    g_object_unref(ostream);
    g_object_unref(istream);
    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv4_store_02)
{
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24);

    GOutputStream  *ostream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mostream =
        G_MEMORY_OUTPUT_STREAM(ostream);

    fail_unless(ipset_save(ostream, &set, NULL),
                "Could not save set");

    GInputStream  *istream =
        g_memory_input_stream_new_from_data
        (g_memory_output_stream_get_data(mostream),
         g_memory_output_stream_get_data_size(mostream),
         NULL);

    read_set = ipset_load(istream, NULL);
    fail_if(read_set == NULL,
            "Could not read set");

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    g_object_unref(ostream);
    g_object_unref(istream);
    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv4_store_03)
{
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv4_add(&set, &IPV4_ADDR_1);
    ipset_ipv4_add(&set, &IPV4_ADDR_2);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_3, 24);

    GOutputStream  *ostream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mostream =
        G_MEMORY_OUTPUT_STREAM(ostream);

    fail_unless(ipset_save(ostream, &set, NULL),
                "Could not save set");

    GInputStream  *istream =
        g_memory_input_stream_new_from_data
        (g_memory_output_stream_get_data(mostream),
         g_memory_output_stream_get_data_size(mostream),
         NULL);

    read_set = ipset_load(istream, NULL);
    fail_if(read_set == NULL,
            "Could not read set");

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    g_object_unref(ostream);
    g_object_unref(istream);
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

#if GLIB_SIZEOF_VOID_P == 4
    expected = 1548;
#elif GLIB_SIZEOF_VOID_P == 8
    expected = 3096;
#else
#   error "Unknown architecture: not 32-bit or 64-bit"
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

#if GLIB_SIZEOF_VOID_P == 4
    expected = 300;
#elif GLIB_SIZEOF_VOID_P == 8
    expected = 600;
#else
#   error "Unknown architecture: not 32-bit or 64-bit"
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
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv6_add(&set, &IPV6_ADDR_1);

    GOutputStream  *ostream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mostream =
        G_MEMORY_OUTPUT_STREAM(ostream);

    fail_unless(ipset_save(ostream, &set, NULL),
                "Could not save set");

    GInputStream  *istream =
        g_memory_input_stream_new_from_data
        (g_memory_output_stream_get_data(mostream),
         g_memory_output_stream_get_data_size(mostream),
         NULL);

    read_set = ipset_load(istream, NULL);
    fail_if(read_set == NULL,
            "Could not read set");

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    g_object_unref(ostream);
    g_object_unref(istream);
    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv6_store_02)
{
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 24);

    GOutputStream  *ostream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mostream =
        G_MEMORY_OUTPUT_STREAM(ostream);

    fail_unless(ipset_save(ostream, &set, NULL),
                "Could not save set");

    GInputStream  *istream =
        g_memory_input_stream_new_from_data
        (g_memory_output_stream_get_data(mostream),
         g_memory_output_stream_get_data_size(mostream),
         NULL);

    read_set = ipset_load(istream, NULL);
    fail_if(read_set == NULL,
            "Could not read set");

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    g_object_unref(ostream);
    g_object_unref(istream);
    ipset_done(&set);
    ipset_free(read_set);
}
END_TEST

START_TEST(test_ipv6_store_03)
{
    ip_set_t  set;
    ip_set_t  *read_set;

    ipset_init(&set);
    ipset_ipv6_add(&set, &IPV6_ADDR_1);
    ipset_ipv6_add(&set, &IPV6_ADDR_2);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_3, 24);

    GOutputStream  *ostream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mostream =
        G_MEMORY_OUTPUT_STREAM(ostream);

    fail_unless(ipset_save(ostream, &set, NULL),
                "Could not save set");

    GInputStream  *istream =
        g_memory_input_stream_new_from_data
        (g_memory_output_stream_get_data(mostream),
         g_memory_output_stream_get_data_size(mostream),
         NULL);

    read_set = ipset_load(istream, NULL);
    fail_if(read_set == NULL,
            "Could not read set");

    fail_unless(ipset_is_equal(&set, read_set),
                "Set not same after saving/loading");

    g_object_unref(ostream);
    g_object_unref(istream);
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
    tcase_add_test(tc_general, test_set_starts_empty);
    tcase_add_test(tc_general, test_empty_sets_equal);
    tcase_add_test(tc_general, test_empty_sets_not_unequal);
    tcase_add_test(tc_general, test_store_empty);
    suite_add_tcase(s, tc_general);

    TCase  *tc_ipv4 = tcase_create("ipv4");
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

    g_type_init();
    ipset_init_library();

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
