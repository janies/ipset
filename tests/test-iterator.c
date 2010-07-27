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
 * Iterators
 */

START_TEST(test_ipv4_iterate_01)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "192.168.0.1");

    fail_if(ipset_ip_add(&set, &ip1),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 element");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_ipv4_iterate_network_01)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "192.168.0.0");

    ipset_ip_t  ip2;
    ipset_ip_from_string(&ip2, "192.168.0.1");

    fail_if(ipset_ip_add_network(&set, &ip1, 31),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");

    ipset_iterator_advance(it);
    fail_if(it->finished,
            "IP set should have more than 1 element");
    fail_unless(ipset_ip_equal(&ip2, &it->addr),
                "IP address 1 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 2 elements");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_ipv6_iterate_01)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "fe80::1");

    fail_if(ipset_ip_add(&set, &ip1),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 element");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_ipv6_iterate_network_01)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "fe80::");

    ipset_ip_t  ip2;
    ipset_ip_from_string(&ip2, "fe80::1");

    fail_if(ipset_ip_add_network(&set, &ip1, 127),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");

    ipset_iterator_advance(it);
    fail_if(it->finished,
            "IP set should have more than 1 element");
    fail_unless(ipset_ip_equal(&ip2, &it->addr),
                "IP address 1 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 2 elements");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
ipset_suite()
{
    Suite  *s = suite_create("ipset");

    TCase  *tc_iterator = tcase_create("iterator");
    tcase_add_test(tc_iterator, test_ipv4_iterate_01);
    tcase_add_test(tc_iterator, test_ipv4_iterate_network_01);
    tcase_add_test(tc_iterator, test_ipv6_iterate_01);
    tcase_add_test(tc_iterator, test_ipv6_iterate_network_01);
    suite_add_tcase(s, tc_iterator);

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
