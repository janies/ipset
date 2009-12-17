/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>

#include <check.h>

#include <ipset/ipset.h>


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
    suite_add_tcase(s, tc_ipv4);

    TCase  *tc_ipv6 = tcase_create("ipv6");
    tcase_add_test(tc_ipv6, test_ipv6_starts_empty);
    tcase_add_test(tc_ipv4, test_ipv6_empty_sets_equal);
    tcase_add_test(tc_ipv4, test_ipv6_empty_sets_not_unequal);
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
