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

#include <ipset/bdd/nodes.h>


/*-----------------------------------------------------------------------
 * BDD terminals
 */

START_TEST(test_bdd_false_terminal)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);

    fail_unless(ipset_node_get_type(n_false) == IPSET_TERMINAL_NODE,
                "False terminal has wrong type");

    fail_unless(ipset_terminal_value(n_false) == FALSE,
                "False terminal has wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_true_terminal)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    fail_unless(ipset_node_get_type(n_true) == IPSET_TERMINAL_NODE,
                "True terminal has wrong type");

    fail_unless(ipset_terminal_value(n_true) == TRUE,
                "True terminal has wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_terminal_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  node1 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  node2 =
        ipset_node_cache_terminal(cache, FALSE);

    fail_unless(node1 == node2,
                "Terminal node isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * BDD non-terminals
 */

START_TEST(test_bdd_nonterminal_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);

    fail_unless(ipset_node_get_type(node) == IPSET_NONTERMINAL_NODE,
                "Nonterminal has wrong type");

    ipset_node_t  *n =
        ipset_nonterminal_node(node);

    fail_unless(n->variable == 0,
                "Nonterminal has wrong variable");
    fail_unless(n->low == n_false,
                "Nonterminal has wrong low pointer");
    fail_unless(n->high == n_true,
                "Nonterminal has wrong high pointer");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_nonterminal_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * If we create nonterminals via a BDD engine, they will be
     * reduced — i.e., every nonterminal with the same value will be
     * in the same memory location.
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  node2 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);

    fail_unless(node1 == node2,
                "Nonterminal node isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_nonterminal_reduced_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * We shouldn't have a nonterminal whose low and high subtrees are
     * equal.
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);

    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_false);

    fail_unless(node == n_false,
                "Nonterminal node isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Testing harness
 */

static Suite *
test_suite()
{
    Suite  *s = suite_create("bdd");

    TCase  *tc_terminals = tcase_create("terminals");
    tcase_add_test(tc_terminals, test_bdd_false_terminal);
    tcase_add_test(tc_terminals, test_bdd_true_terminal);
    tcase_add_test(tc_terminals, test_bdd_terminal_reduced_1);
    suite_add_tcase(s, tc_terminals);

    TCase  *tc_nonterminals = tcase_create("nonterminals");
    tcase_add_test(tc_nonterminals, test_bdd_nonterminal_1);
    tcase_add_test(tc_nonterminals, test_bdd_nonterminal_reduced_1);
    tcase_add_test(tc_nonterminals, test_bdd_nonterminal_reduced_2);
    suite_add_tcase(s, tc_nonterminals);

    return s;
}


int
main(int argc, const char **argv)
{
    int  number_failed;
    Suite  *suite = test_suite();
    SRunner  *runner = srunner_create(suite);

    srunner_run_all(runner, CK_NORMAL);
    number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
