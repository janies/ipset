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


/*-----------------------------------------------------------------------
 * IPv6 tests
 */


/*-----------------------------------------------------------------------
 * Testing harness
 */

Suite *
ipset_suite()
{
    Suite  *s = suite_create("ipset");

    TCase  *tc_ipv4 = tcase_create("ipv4");
    suite_add_tcase(s, tc_ipv4);

    TCase  *tc_ipv6 = tcase_create("ipv6");
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
