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

#include <ipset/ip.h>


/*-----------------------------------------------------------------------
 * Sample IP addresses
 */

typedef guint8  ipv4_addr_t[4];
typedef guint8  ipv6_addr_t[16];

static ipv4_addr_t  IPV4_ADDR_1 = "\xc0\xa8\x01\x64"; /* 192.168.1.100 */

static ipv6_addr_t  IPV6_ADDR_1 =
"\xfe\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01";
static ipv6_addr_t  IPV6_ADDR_2 =
"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xff\xff\xc0\xa8\x01\x64";


/*-----------------------------------------------------------------------
 * IPv4
 */

START_TEST(test_ipv4_parse_01)
{
    ipset_ip_t  ip1;
    ipset_ip_t  ip2;

    ipset_ip_from_ipv4(&ip1, IPV4_ADDR_1);
    ipset_ip_from_string(&ip2, "192.168.1.100");

    fail_unless(ipset_ip_equal(&ip1, &ip2),
                "IPv4 addresses should be equal");
}
END_TEST


/*-----------------------------------------------------------------------
 * IPv6
 */

START_TEST(test_ipv6_parse_01)
{
    ipset_ip_t  ip1;
    ipset_ip_t  ip2;

    ipset_ip_from_ipv6(&ip1, IPV6_ADDR_1);
    ipset_ip_from_string(&ip2, "fe80::1");

    fail_unless(ipset_ip_equal(&ip1, &ip2),
                "IPv6 addresses should be equal");
}
END_TEST


START_TEST(test_ipv6_parse_02)
{
    ipset_ip_t  ip1;
    ipset_ip_t  ip2;

    ipset_ip_from_ipv6(&ip1, IPV6_ADDR_2);
    ipset_ip_from_string(&ip2, "::ffff:192.168.1.100");

    fail_unless(ipset_ip_equal(&ip1, &ip2),
                "IPv6 addresses should be equal");
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
ipset_suite()
{
    Suite  *s = suite_create("ip");

    TCase  *tc_ipv4 = tcase_create("ipv4");
    tcase_add_test(tc_ipv4, test_ipv4_parse_01);
    suite_add_tcase(s, tc_ipv4);

    TCase  *tc_ipv6 = tcase_create("ipv6");
    tcase_add_test(tc_ipv6, test_ipv6_parse_01);
    tcase_add_test(tc_ipv6, test_ipv6_parse_02);
    suite_add_tcase(s, tc_ipv6);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = ipset_suite();
    SRunner  *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
