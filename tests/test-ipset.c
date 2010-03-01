/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdint.h>
#include <stdlib.h>

#include <check.h>

#include <ipset/ipset.h>


/*-----------------------------------------------------------------------
 * Sample IP addresses
 */

typedef uint32_t  ipv4_addr_t;
typedef uint32_t  ipv6_addr_t[4];

static ipv4_addr_t  IPV4_ADDR_1 = 0xc0a80164;    /* 192.168.1.100 */

static ipv6_addr_t  IPV6_ADDR_1 =
{ 0xfe800000, 0x00000000, 0x021ec2ff, 0xfe9fe8e1 };

/*-----------------------------------------------------------------------
 * IPv4 tests
 */

START_TEST(test_ipv4_starts_empty)
{
    ipv4_set_t  set;

    ipset_ipv4_init(&set);
    fail_unless(ipset_ipv4_is_empty(&set),
                "IPv4 set should start empty");
    ipset_ipv4_done(&set);
}
END_TEST

START_TEST(test_ipv4_empty_sets_equal)
{
    ipv4_set_t  set1, set2;

    ipset_ipv4_init(&set1);
    ipset_ipv4_init(&set2);
    fail_unless(ipset_ipv4_is_equal(&set1, &set2),
                "Empty IPv4 sets should be equal");
    ipset_ipv4_done(&set1);
    ipset_ipv4_done(&set2);
}
END_TEST

START_TEST(test_ipv4_empty_sets_not_unequal)
{
    ipv4_set_t  set1, set2;

    ipset_ipv4_init(&set1);
    ipset_ipv4_init(&set2);
    fail_if(ipset_ipv4_is_not_equal(&set1, &set2),
            "Empty IPv4 sets should not be unequal");
    ipset_ipv4_done(&set1);
    ipset_ipv4_done(&set2);
}
END_TEST

START_TEST(test_ipv4_insert)
{
    ipv4_set_t  set;

    ipset_ipv4_init(&set);

    fail_if(ipset_ipv4_add(&set, &IPV4_ADDR_1),
            "Element should not be present");

    fail_unless(ipset_ipv4_add(&set, &IPV4_ADDR_1),
                "Element should be present");

    ipset_ipv4_done(&set);
}
END_TEST

START_TEST(test_ipv4_insert_network)
{
    ipv4_set_t  set;

    ipset_ipv4_init(&set);

    fail_if(ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24),
            "Element should not be present");

    fail_unless(ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24),
                "Element should be present");

    ipset_ipv4_done(&set);
}
END_TEST

START_TEST(test_ipv4_equality_1)
{
    ipv4_set_t  set1, set2;

    ipset_ipv4_init(&set1);
    ipset_ipv4_add(&set1, &IPV4_ADDR_1);

    ipset_ipv4_init(&set2);
    ipset_ipv4_add(&set2, &IPV4_ADDR_1);

    fail_unless(ipset_ipv4_is_equal(&set1, &set2),
                "Expected {x} == {x}");

    ipset_ipv4_done(&set1);
    ipset_ipv4_done(&set2);
}
END_TEST

START_TEST(test_ipv4_inequality_1)
{
    ipv4_set_t  set1, set2;

    ipset_ipv4_init(&set1);
    ipset_ipv4_add(&set1, &IPV4_ADDR_1);

    ipset_ipv4_init(&set2);
    ipset_ipv4_add_network(&set2, &IPV4_ADDR_1, 24);

    fail_unless(ipset_ipv4_is_not_equal(&set1, &set2),
                "Expected {x} != {x}");

    ipset_ipv4_done(&set1);
    ipset_ipv4_done(&set2);
}
END_TEST

START_TEST(test_ipv4_memory_size_1)
{
    ipv4_set_t  set;
    size_t  expected, actual;

    ipset_ipv4_init(&set);
    ipset_ipv4_add(&set, &IPV4_ADDR_1);

    expected = 1056;
    actual = ipset_ipv4_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_ipv4_done(&set);
}
END_TEST

START_TEST(test_ipv4_memory_size_2)
{
    ipv4_set_t  set;
    size_t  expected, actual;

    ipset_ipv4_init(&set);
    ipset_ipv4_add_network(&set, &IPV4_ADDR_1, 24);

    expected = 800;
    actual = ipset_ipv4_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_ipv4_done(&set);
}
END_TEST


/*-----------------------------------------------------------------------
 * IPv6 tests
 */

START_TEST(test_ipv6_starts_empty)
{
    ipv6_set_t  set;

    ipset_ipv6_init(&set);
    fail_unless(ipset_ipv6_is_empty(&set),
                "IPv6 set should start empty");
    ipset_ipv6_done(&set);
}
END_TEST

START_TEST(test_ipv6_empty_sets_equal)
{
    ipv6_set_t  set1, set2;

    ipset_ipv6_init(&set1);
    ipset_ipv6_init(&set2);
    fail_unless(ipset_ipv6_is_equal(&set1, &set2),
                "Empty IPv6 sets should be equal");
    ipset_ipv6_done(&set1);
    ipset_ipv6_done(&set2);
}
END_TEST

START_TEST(test_ipv6_empty_sets_not_unequal)
{
    ipv6_set_t  set1, set2;

    ipset_ipv6_init(&set1);
    ipset_ipv6_init(&set2);
    fail_if(ipset_ipv6_is_not_equal(&set1, &set2),
            "Empty IPv6 sets should not be unequal");
    ipset_ipv6_done(&set1);
    ipset_ipv6_done(&set2);
}
END_TEST

START_TEST(test_ipv6_insert)
{
    ipv6_set_t  set;

    ipset_ipv6_init(&set);

    fail_if(ipset_ipv6_add(&set, &IPV6_ADDR_1),
            "Element should not be present");

    fail_unless(ipset_ipv6_add(&set, &IPV6_ADDR_1),
                "Element should be present");

    ipset_ipv6_done(&set);
}
END_TEST

START_TEST(test_ipv6_insert_network)
{
    ipv6_set_t  set;

    ipset_ipv6_init(&set);

    fail_if(ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 32),
            "Element should not be present");

    fail_unless(ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 32),
                "Element should be present");

    ipset_ipv6_done(&set);
}
END_TEST

START_TEST(test_ipv6_equality_1)
{
    ipv6_set_t  set1, set2;

    ipset_ipv6_init(&set1);
    ipset_ipv6_add(&set1, &IPV6_ADDR_1);

    ipset_ipv6_init(&set2);
    ipset_ipv6_add(&set2, &IPV6_ADDR_1);

    fail_unless(ipset_ipv6_is_equal(&set1, &set2),
                "Expected {x} == {x}");

    ipset_ipv6_done(&set1);
    ipset_ipv6_done(&set2);
}
END_TEST

START_TEST(test_ipv6_inequality_1)
{
    ipv6_set_t  set1, set2;

    ipset_ipv6_init(&set1);
    ipset_ipv6_add(&set1, &IPV6_ADDR_1);

    ipset_ipv6_init(&set2);
    ipset_ipv6_add_network(&set2, &IPV6_ADDR_1, 32);

    fail_unless(ipset_ipv6_is_not_equal(&set1, &set2),
                "Expected {x} != {x}");

    ipset_ipv6_done(&set1);
    ipset_ipv6_done(&set2);
}
END_TEST

START_TEST(test_ipv6_memory_size_1)
{
    ipv6_set_t  set;
    size_t  expected, actual;

    ipset_ipv6_init(&set);
    ipset_ipv6_add(&set, &IPV6_ADDR_1);

    expected = 4128;
    actual = ipset_ipv6_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_ipv6_done(&set);
}
END_TEST

START_TEST(test_ipv6_memory_size_2)
{
    ipv6_set_t  set;
    size_t  expected, actual;

    ipset_ipv6_init(&set);
    ipset_ipv6_add_network(&set, &IPV6_ADDR_1, 24);

    expected = 800;
    actual = ipset_ipv6_memory_size(&set);

    fail_unless(expected == actual,
                "Expected set to be %zu bytes, got %zu bytes",
                expected, actual);

    ipset_ipv6_done(&set);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
ipset_suite()
{
    Suite  *s = suite_create("ipset");

    TCase  *tc_ipv4 = tcase_create("ipv4");
    tcase_add_test(tc_ipv4, test_ipv4_starts_empty);
    tcase_add_test(tc_ipv4, test_ipv4_empty_sets_equal);
    tcase_add_test(tc_ipv4, test_ipv4_empty_sets_not_unequal);
    tcase_add_test(tc_ipv4, test_ipv4_insert);
    tcase_add_test(tc_ipv4, test_ipv4_insert_network);
    tcase_add_test(tc_ipv4, test_ipv4_equality_1);
    tcase_add_test(tc_ipv4, test_ipv4_inequality_1);
    tcase_add_test(tc_ipv4, test_ipv4_memory_size_1);
    tcase_add_test(tc_ipv4, test_ipv4_memory_size_2);
    suite_add_tcase(s, tc_ipv4);

    TCase  *tc_ipv6 = tcase_create("ipv6");
    tcase_add_test(tc_ipv6, test_ipv6_starts_empty);
    tcase_add_test(tc_ipv4, test_ipv6_empty_sets_equal);
    tcase_add_test(tc_ipv4, test_ipv6_empty_sets_not_unequal);
    tcase_add_test(tc_ipv4, test_ipv6_insert);
    tcase_add_test(tc_ipv4, test_ipv6_insert_network);
    tcase_add_test(tc_ipv4, test_ipv6_equality_1);
    tcase_add_test(tc_ipv4, test_ipv6_inequality_1);
    tcase_add_test(tc_ipv4, test_ipv6_memory_size_1);
    tcase_add_test(tc_ipv4, test_ipv6_memory_size_2);
    suite_add_tcase(s, tc_ipv6);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = ipset_suite();
    SRunner  *runner = srunner_create(suite);

    ipset_init();

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
