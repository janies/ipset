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
#include <string.h>

#include <check.h>
#include <glib.h>

#include <ipset/bdd/nodes.h>


/*-----------------------------------------------------------------------
 * Assignments
 */

START_TEST(test_bdd_assignment_empty_equal)
{
    ipset_assignment_t  *a1;
    ipset_assignment_t  *a2;

    a1 = ipset_assignment_new();
    a2 = ipset_assignment_new();

    fail_unless(ipset_assignment_equal(a1, a2),
                "Assignments should be equal");

    ipset_assignment_free(a1);
    ipset_assignment_free(a2);
}
END_TEST


START_TEST(test_bdd_assignment_equal_1)
{
    ipset_assignment_t  *a1;
    ipset_assignment_t  *a2;

    a1 = ipset_assignment_new();
    ipset_assignment_set(a1, 0, IPSET_TRUE);
    ipset_assignment_set(a1, 1, IPSET_FALSE);

    a2 = ipset_assignment_new();
    ipset_assignment_set(a2, 0, IPSET_TRUE);
    ipset_assignment_set(a2, 1, IPSET_FALSE);

    fail_unless(ipset_assignment_equal(a1, a2),
                "Assignments should be equal");

    ipset_assignment_free(a1);
    ipset_assignment_free(a2);
}
END_TEST


START_TEST(test_bdd_assignment_equal_2)
{
    ipset_assignment_t  *a1;
    ipset_assignment_t  *a2;

    a1 = ipset_assignment_new();
    ipset_assignment_set(a1, 0, IPSET_TRUE);
    ipset_assignment_set(a1, 1, IPSET_FALSE);

    a2 = ipset_assignment_new();
    ipset_assignment_set(a2, 0, IPSET_TRUE);
    ipset_assignment_set(a2, 1, IPSET_FALSE);
    ipset_assignment_set(a2, 4, IPSET_EITHER);

    fail_unless(ipset_assignment_equal(a1, a2),
                "Assignments should be equal");

    ipset_assignment_free(a1);
    ipset_assignment_free(a2);
}
END_TEST


START_TEST(test_bdd_assignment_cut_1)
{
    ipset_assignment_t  *a1;
    ipset_assignment_t  *a2;

    a1 = ipset_assignment_new();
    ipset_assignment_set(a1, 0, IPSET_TRUE);
    ipset_assignment_set(a1, 1, IPSET_FALSE);

    a2 = ipset_assignment_new();
    ipset_assignment_set(a2, 0, IPSET_TRUE);
    ipset_assignment_set(a2, 1, IPSET_FALSE);
    ipset_assignment_set(a2, 2, IPSET_TRUE);
    ipset_assignment_set(a2, 3, IPSET_TRUE);
    ipset_assignment_set(a2, 4, IPSET_FALSE);

    ipset_assignment_cut(a2, 2);

    fail_unless(ipset_assignment_equal(a1, a2),
                "Assignments should be equal");

    ipset_assignment_free(a1);
    ipset_assignment_free(a2);
}
END_TEST


/*-----------------------------------------------------------------------
 * Expanded assignments
 */

START_TEST(test_bdd_assignment_expand_1)
{
    ipset_assignment_t  *a;

    a = ipset_assignment_new();
    ipset_assignment_set(a, 0, TRUE);
    ipset_assignment_set(a, 1, FALSE);

    ipset_expanded_assignment_t  *it;
    it = ipset_assignment_expand(a, 2);

    GByteArray  *ea = g_byte_array_sized_new(1);
    memset(ea->data, 0, 1);

    fail_if(it->finished,
            "Expanded assignment shouldn't be empty");
    IPSET_BIT_SET(ea->data, 0, TRUE);
    IPSET_BIT_SET(ea->data, 1, FALSE);
    fail_unless(memcmp(ea->data, it->values->data, 1) == 0,
                "Expanded assignment doesn't match");

    ipset_expanded_assignment_advance(it);
    fail_unless(it->finished,
                "Expanded assignment should have 1 element");

    g_byte_array_free(ea, TRUE);
    ipset_expanded_assignment_free(it);
    ipset_assignment_free(a);
}
END_TEST


START_TEST(test_bdd_assignment_expand_2)
{
    ipset_assignment_t  *a;

    a = ipset_assignment_new();
    ipset_assignment_set(a, 0, TRUE);
    ipset_assignment_set(a, 1, FALSE);

    ipset_expanded_assignment_t  *it;
    it = ipset_assignment_expand(a, 3);

    GByteArray  *ea = g_byte_array_sized_new(1);
    memset(ea->data, 0, 1);

    fail_if(it->finished,
            "Expanded assignment shouldn't be empty");
    IPSET_BIT_SET(ea->data, 0, TRUE);
    IPSET_BIT_SET(ea->data, 1, FALSE);
    IPSET_BIT_SET(ea->data, 2, FALSE);
    fail_unless(memcmp(ea->data, it->values->data, 1) == 0,
                "Expanded assignment 1 doesn't match");

    ipset_expanded_assignment_advance(it);
    fail_if(it->finished,
                "Expanded assignment should have at least 1 element");
    IPSET_BIT_SET(ea->data, 0, TRUE);
    IPSET_BIT_SET(ea->data, 1, FALSE);
    IPSET_BIT_SET(ea->data, 2, TRUE);
    fail_unless(memcmp(ea->data, it->values->data, 1) == 0,
                "Expanded assignment 2 doesn't match");

    ipset_expanded_assignment_advance(it);
    fail_unless(it->finished,
                "Expanded assignment should have 2 elements");

    g_byte_array_free(ea, TRUE);
    ipset_expanded_assignment_free(it);
    ipset_assignment_free(a);
}
END_TEST


START_TEST(test_bdd_assignment_expand_3)
{
    ipset_assignment_t  *a;

    a = ipset_assignment_new();
    ipset_assignment_set(a, 0, TRUE);
    ipset_assignment_set(a, 2, FALSE);

    ipset_expanded_assignment_t  *it;
    it = ipset_assignment_expand(a, 3);

    GByteArray  *ea = g_byte_array_sized_new(1);
    memset(ea->data, 0, 1);

    fail_if(it->finished,
            "Expanded assignment shouldn't be empty");
    IPSET_BIT_SET(ea->data, 0, TRUE);
    IPSET_BIT_SET(ea->data, 1, FALSE);
    IPSET_BIT_SET(ea->data, 2, FALSE);
    fail_unless(memcmp(ea->data, it->values->data, 1) == 0,
                "Expanded assignment 1 doesn't match");

    ipset_expanded_assignment_advance(it);
    fail_if(it->finished,
                "Expanded assignment should have at least 1 element");
    IPSET_BIT_SET(ea->data, 0, TRUE);
    IPSET_BIT_SET(ea->data, 1, TRUE);
    IPSET_BIT_SET(ea->data, 2, FALSE);
    fail_unless(memcmp(ea->data, it->values->data, 1) == 0,
                "Expanded assignment 2 doesn't match");

    ipset_expanded_assignment_advance(it);
    fail_unless(it->finished,
                "Expanded assignment should have 2 elements");

    g_byte_array_free(ea, TRUE);
    ipset_expanded_assignment_free(it);
    ipset_assignment_free(a);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

static Suite *
test_suite()
{
    Suite  *s = suite_create("assignment");

    TCase  *tc_assignments = tcase_create("assignments");
    tcase_add_test(tc_assignments, test_bdd_assignment_empty_equal);
    tcase_add_test(tc_assignments, test_bdd_assignment_equal_1);
    tcase_add_test(tc_assignments, test_bdd_assignment_equal_2);
    tcase_add_test(tc_assignments, test_bdd_assignment_cut_1);
    suite_add_tcase(s, tc_assignments);

    TCase  *tc_expanded = tcase_create("expanded");
    tcase_add_test(tc_expanded, test_bdd_assignment_expand_1);
    tcase_add_test(tc_expanded, test_bdd_assignment_expand_2);
    tcase_add_test(tc_expanded, test_bdd_assignment_expand_3);
    suite_add_tcase(s, tc_expanded);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    g_type_init();

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
