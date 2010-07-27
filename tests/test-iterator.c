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

START_TEST(test_iterate_empty)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_iterator_t  *it = ipset_iterate(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_unless(it->finished,
                "IP set should be empty");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


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
    fail_unless(it->netmask == IPV4_BIT_SIZE,
                "IP netmask 0 doesn't match");

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

    fail_if(ipset_ip_add_network(&set, &ip1, 31),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 31,
                "IP netmask 0 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 elements");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_ipv4_iterate_network_02)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "192.168.0.0");

    fail_if(ipset_ip_add_network(&set, &ip1, 16),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 16,
                "IP netmask 0 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 elements");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_ipv4_iterate_network_03)
{
    ip_set_t  set;
    ipset_init(&set);

    /*
     * If we add all of the IP addresses in a network individually, we
     * should still get the network as a whole from the iterator.
     */

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "192.168.0.0");

    ipset_ip_t  ip2;
    ipset_ip_from_string(&ip2, "192.168.0.1");

    fail_if(ipset_ip_add(&set, &ip1),
            "Element should not be present");

    fail_if(ipset_ip_add(&set, &ip2),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 31,
                "IP netmask 0 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 elements");

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
    fail_unless(it->netmask == IPV6_BIT_SIZE,
                "IP netmask 0 doesn't match");

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

    fail_if(ipset_ip_add_network(&set, &ip1, 127),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 127,
                "IP netmask 0 doesn't match (%u)", it->netmask);

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 element");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_ipv6_iterate_network_02)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "fe80::");

    fail_if(ipset_ip_add_network(&set, &ip1, 16),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 16,
                "IP netmask 0 doesn't match (%u)", it->netmask);

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 element");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_ipv6_iterate_network_03)
{
    ip_set_t  set;
    ipset_init(&set);

    /*
     * If we add all of the IP addresses in a network individually, we
     * should still get the network as a whole from the iterator.
     */

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "fe80::");

    ipset_ip_t  ip2;
    ipset_ip_from_string(&ip2, "fe80::1");

    fail_if(ipset_ip_add(&set, &ip1),
            "Element should not be present");

    fail_if(ipset_ip_add(&set, &ip2),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 127,
                "IP netmask 0 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 1 elements");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_generic_ip_iterate_01)
{
    ip_set_t  set;
    ipset_init(&set);

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "0.0.0.0");

    ipset_ip_t  ip2;
    ipset_ip_from_string(&ip2, "::");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, FALSE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 0,
                "IP netmask 0 doesn't match");

    ipset_iterator_advance(it);
    fail_if(it->finished,
            "IP set should have more than 1 element");
    fail_unless(ipset_ip_equal(&ip2, &it->addr),
                "IP address 1 doesn't match");
    fail_unless(it->netmask == 0,
                "IP netmask 1 doesn't match");

    ipset_iterator_advance(it);
    fail_unless(it->finished,
                "IP set should contain 2 elements");

    ipset_iterator_free(it);

    ipset_done(&set);
}
END_TEST


START_TEST(test_generic_ip_iterate_02)
{
    ip_set_t  set;
    ipset_init(&set);

    /*
     * These addresses are carefully constructed so that the same BDD
     * variable assignments are used to store both, apart from the
     * IPv4/v6 discriminator variable.  The goal is get a BDD that has
     * EITHER in the assignment for variable 0, but isn't simply the
     * empty or full set.
     */

    ipset_ip_t  ip1;
    ipset_ip_from_string(&ip1, "192.168.0.1"); /* 0xc0a80001 */

    ipset_ip_t  ip2;
    ipset_ip_from_string(&ip2, "c0a8:0001::");

    fail_if(ipset_ip_add(&set, &ip1),
            "Element should not be present");
    fail_if(ipset_ip_add_network(&set, &ip2, 32),
            "Element should not be present");

    ipset_iterator_t  *it = ipset_iterate_networks(&set, TRUE);
    fail_if(it == NULL,
            "IP set iterator is NULL");

    fail_if(it->finished,
            "IP set shouldn't be empty");
    fail_unless(ipset_ip_equal(&ip1, &it->addr),
                "IP address 0 doesn't match");
    fail_unless(it->netmask == 32,
                "IP netmask 0 doesn't match");

    ipset_iterator_advance(it);
    fail_if(it->finished,
            "IP set should have more than 1 element");
    fail_unless(ipset_ip_equal(&ip2, &it->addr),
                "IP address 1 doesn't match");
    fail_unless(it->netmask == 32,
                "IP netmask 1 doesn't match");

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
    tcase_add_test(tc_iterator, test_iterate_empty);
    tcase_add_test(tc_iterator, test_ipv4_iterate_01);
    tcase_add_test(tc_iterator, test_ipv4_iterate_network_01);
    tcase_add_test(tc_iterator, test_ipv4_iterate_network_02);
    tcase_add_test(tc_iterator, test_ipv4_iterate_network_03);
    tcase_add_test(tc_iterator, test_ipv6_iterate_01);
    tcase_add_test(tc_iterator, test_ipv6_iterate_network_01);
    tcase_add_test(tc_iterator, test_ipv6_iterate_network_02);
    tcase_add_test(tc_iterator, test_ipv6_iterate_network_03);
    tcase_add_test(tc_iterator, test_generic_ip_iterate_01);
    tcase_add_test(tc_iterator, test_generic_ip_iterate_02);
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
