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
 * Evaluation
 */

START_TEST(test_bdd_evaluate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = ¬x[0]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);

    /*
     * And test we can get the right results out of it.
     */

    guint8  input1[] = { 0x80 }; /* { TRUE } */
    gboolean  expected1 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bit_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    guint8  input2[] = { 0x00 }; /* { FALSE } */
    gboolean  expected2 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bit_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_evaluate_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = ¬x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node =
        ipset_node_cache_nonterminal(cache, 0, node1, n_false);

    /*
     * And test we can get the right results out of it.
     */

    gboolean  input1[] = { TRUE, TRUE };
    gboolean  expected1 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    gboolean  input2[] = { TRUE, FALSE };
    gboolean  expected2 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    gboolean  input3[] = { FALSE, TRUE };
    gboolean  expected3 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input3)
                == expected3,
                "BDD evaluates to wrong value");

    gboolean  input4[] = { FALSE, FALSE };
    gboolean  expected4 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input4)
                == expected4,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Operators
 */

START_TEST(test_bdd_and_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false0 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true0 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node00 =
        ipset_node_cache_nonterminal(cache, 0, n_false0, n_true0);
    ipset_node_id_t  node01 =
        ipset_node_cache_nonterminal(cache, 1, n_false0, n_true0);
    ipset_node_id_t  node0 =
        ipset_node_cache_and(cache, node00, node01);

    /*
     * And then do it again.
     */

    ipset_node_id_t  n_false1 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true1 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node10 =
        ipset_node_cache_nonterminal(cache, 0, n_false1, n_true1);
    ipset_node_id_t  node11 =
        ipset_node_cache_nonterminal(cache, 1, n_false1, n_true1);
    ipset_node_id_t  node1 =
        ipset_node_cache_and(cache, node10, node11);

    /*
     * Verify that we get the same physical node both times.
     */

    fail_unless(node0 == node1,
                "AND operator result isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_and_evaluate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node =
        ipset_node_cache_and(cache, node0, node1);

    /*
     * And test we can get the right results out of it.
     */

    gboolean  input1[] = { TRUE, TRUE };
    gboolean  expected1 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    gboolean  input2[] = { TRUE, FALSE };
    gboolean  expected2 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    gboolean  input3[] = { FALSE, TRUE };
    gboolean  expected3 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input3)
                == expected3,
                "BDD evaluates to wrong value");

    gboolean  input4[] = { FALSE, FALSE };
    gboolean  expected4 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input4)
                == expected4,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_or_reduced_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false0 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true0 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node00 =
        ipset_node_cache_nonterminal(cache, 0, n_false0, n_true0);
    ipset_node_id_t  node01 =
        ipset_node_cache_nonterminal(cache, 1, n_false0, n_true0);
    ipset_node_id_t  node0 =
        ipset_node_cache_or(cache, node00, node01);

    /*
     * And then do it again.
     */

    ipset_node_id_t  n_false1 =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true1 =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node10 =
        ipset_node_cache_nonterminal(cache, 0, n_false1, n_true1);
    ipset_node_id_t  node11 =
        ipset_node_cache_nonterminal(cache, 1, n_false1, n_true1);
    ipset_node_id_t  node1 =
        ipset_node_cache_or(cache, node10, node11);

    /*
     * Verify that we get the same physical node both times.
     */

    fail_unless(node0 == node1,
                "OR operator result isn't reduced");

    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_or_evaluate_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = x[0] ∧ x[1]
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  node0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  node1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, node0, node1);

    /*
     * And test we can get the right results out of it.
     */

    gboolean  input1[] = { TRUE, TRUE };
    gboolean  expected1 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input1)
                == expected1,
                "BDD evaluates to wrong value");

    gboolean  input2[] = { TRUE, FALSE };
    gboolean  expected2 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input2)
                == expected2,
                "BDD evaluates to wrong value");

    gboolean  input3[] = { FALSE, TRUE };
    gboolean  expected3 = TRUE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input3)
                == expected3,
                "BDD evaluates to wrong value");

    gboolean  input4[] = { FALSE, FALSE };
    gboolean  expected4 = FALSE;

    fail_unless(ipset_node_evaluate(node,
                                    ipset_bool_array_assignment,
                                    input4)
                == expected4,
                "BDD evaluates to wrong value");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Memory size
 */

START_TEST(test_bdd_size_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  t0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  f0 =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);
    ipset_node_id_t  t1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  t2 =
        ipset_node_cache_nonterminal(cache, 2, n_false, n_true);

    ipset_node_id_t  n1 =
        ipset_node_cache_and(cache, t0, t1);
    ipset_node_id_t  n2 =
        ipset_node_cache_and(cache, f0, t2);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, n1, n2);

    /*
     * And verify how big it is.
     */

    fail_unless(ipset_node_reachable_count(node) == 3u,
                "BDD has wrong number of nodes");

    fail_unless(ipset_node_memory_size(node) ==
                3u * sizeof(ipset_node_t),
                "BDD takes up wrong amount of space");

    ipset_node_cache_free(cache);
}
END_TEST


/*-----------------------------------------------------------------------
 * Serialization
 */

START_TEST(test_bdd_save_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = TRUE
     */

    ipset_node_id_t  node =
        ipset_node_cache_terminal(cache, TRUE);

    /*
     * Serialize the BDD into a string.
     */

    GOutputStream  *stream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mstream =
        G_MEMORY_OUTPUT_STREAM(stream);

    fail_unless(ipset_node_save(stream, node, NULL),
                "Cannot serialize BDD");

    const char  *raw_expected =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x18"   // length
        "\x00\x00\x00\x00"                   // node count
        "\x00\x00\x00\x01"                   // terminal value
        ;
    const size_t  expected_length = 24;

    gpointer  buf = g_memory_output_stream_get_data(mstream);
    gsize  len = g_memory_output_stream_get_data_size(mstream);

    fail_unless(expected_length == len,
                "Serialized BDD has wrong length "
                "(expected %zu, got %zu)",
                expected_length, len);

    fail_unless(memcmp(raw_expected, buf, expected_length) == 0,
                "Serialized BDD has incorrect data");

    g_object_unref(stream);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_save_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  t0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  f0 =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);
    ipset_node_id_t  t1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  t2 =
        ipset_node_cache_nonterminal(cache, 2, n_false, n_true);

    ipset_node_id_t  n1 =
        ipset_node_cache_and(cache, t0, t1);
    ipset_node_id_t  n2 =
        ipset_node_cache_and(cache, f0, t2);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, n1, n2);

    /*
     * Serialize the BDD into a string.
     */

    GOutputStream  *stream =
        g_memory_output_stream_new(NULL, 0, g_realloc, g_free);
    GMemoryOutputStream  *mstream =
        G_MEMORY_OUTPUT_STREAM(stream);

    fail_unless(ipset_node_save(stream, node, NULL),
                "Cannot serialize BDD");

    const char  *raw_expected =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x2f"   // length
        "\x00\x00\x00\x03"                   // node count
        // node -1
        "\x02"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -2
        "\x01"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -3
        "\x00"                               // variable
        "\xff\xff\xff\xff"                   // low
        "\xff\xff\xff\xfe"                   // high
        ;
    const size_t  expected_length = 47;

    gpointer  buf = g_memory_output_stream_get_data(mstream);
    gsize  len = g_memory_output_stream_get_data_size(mstream);

    fail_unless(expected_length == len,
                "Serialized BDD has wrong length "
                "(expected %zu, got %zu)",
                expected_length, len);

    fail_unless(memcmp(raw_expected, buf, expected_length) == 0,
                "Serialized BDD has incorrect data");

    g_object_unref(stream);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_bad_save_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = TRUE
     */

    ipset_node_id_t  node =
        ipset_node_cache_terminal(cache, TRUE);

    /*
     * Serialize the BDD into a buffer that's too small, and verify
     * that we get an error.
     */

    const gsize  buf_len = 16;
    guint8  buf[16];

    GOutputStream  *stream =
        g_memory_output_stream_new(buf, buf_len, NULL, NULL);

    GError  *error = NULL;
    ipset_node_save(stream, node, &error);

    fail_unless(error != NULL,
                "Should get error when serializing "
                "into a small buffer");

    g_error_free(error);
    g_object_unref(stream);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_load_1)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = TRUE
     */

    ipset_node_id_t  node =
        ipset_node_cache_terminal(cache, TRUE);

    /*
     * Read a BDD from a string.
     */

    const char  *raw =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x18"   // length
        "\x00\x00\x00\x00"                   // node count
        "\x00\x00\x00\x01"                   // terminal value
        ;
    const size_t  raw_length = 24;

    GInputStream  *stream =
        g_memory_input_stream_new_from_data
        (raw, raw_length, NULL);

    GError  *error = NULL;
    ipset_node_id_t  read =
        ipset_node_cache_load(stream, cache, &error);

    fail_unless(error == NULL,
                "Error reading BDD from stream");

    fail_unless(read == node,
                "BDD from stream doesn't match expected");

    g_object_unref(stream);
    ipset_node_cache_free(cache);
}
END_TEST


START_TEST(test_bdd_load_2)
{
    ipset_node_cache_t  *cache = ipset_node_cache_new();

    /*
     * Create a BDD representing
     *   f(x) = (x[0] ∧ x[1]) ∨ (¬x[0] ∧ x[2])
     */

    ipset_node_id_t  n_false =
        ipset_node_cache_terminal(cache, FALSE);
    ipset_node_id_t  n_true =
        ipset_node_cache_terminal(cache, TRUE);

    ipset_node_id_t  t0 =
        ipset_node_cache_nonterminal(cache, 0, n_false, n_true);
    ipset_node_id_t  f0 =
        ipset_node_cache_nonterminal(cache, 0, n_true, n_false);
    ipset_node_id_t  t1 =
        ipset_node_cache_nonterminal(cache, 1, n_false, n_true);
    ipset_node_id_t  t2 =
        ipset_node_cache_nonterminal(cache, 2, n_false, n_true);

    ipset_node_id_t  n1 =
        ipset_node_cache_and(cache, t0, t1);
    ipset_node_id_t  n2 =
        ipset_node_cache_and(cache, f0, t2);
    ipset_node_id_t  node =
        ipset_node_cache_or(cache, n1, n2);

    /*
     * Read a BDD from a string.
     */

    const char  *raw =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x2f"   // length
        "\x00\x00\x00\x03"                   // node count
        // node -1
        "\x02"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -2
        "\x01"                               // variable
        "\x00\x00\x00\x00"                   // low
        "\x00\x00\x00\x01"                   // high
        // node -3
        "\x00"                               // variable
        "\xff\xff\xff\xff"                   // low
        "\xff\xff\xff\xfe"                   // high
        ;
    const size_t  raw_length = 47;

    GInputStream  *stream =
        g_memory_input_stream_new_from_data
        (raw, raw_length, NULL);

    GError  *error = NULL;
    ipset_node_id_t  read =
        ipset_node_cache_load(stream, cache, &error);

    fail_unless(error == NULL,
                "Error reading BDD from stream");

    fail_unless(read == node,
                "BDD from stream doesn't match expected");

    g_object_unref(stream);
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

    TCase  *tc_evaluation = tcase_create("evaluation");
    tcase_add_test(tc_evaluation, test_bdd_evaluate_1);
    tcase_add_test(tc_evaluation, test_bdd_evaluate_2);
    suite_add_tcase(s, tc_evaluation);

    TCase  *tc_operators = tcase_create("operators");
    tcase_add_test(tc_operators, test_bdd_and_reduced_1);
    tcase_add_test(tc_operators, test_bdd_and_evaluate_1);
    tcase_add_test(tc_operators, test_bdd_or_reduced_1);
    tcase_add_test(tc_operators, test_bdd_or_evaluate_1);
    suite_add_tcase(s, tc_operators);

    TCase  *tc_size = tcase_create("size");
    tcase_add_test(tc_size, test_bdd_size_1);
    suite_add_tcase(s, tc_size);

    TCase  *tc_serialization = tcase_create("serialization");
    tcase_add_test(tc_serialization, test_bdd_save_1);
    tcase_add_test(tc_serialization, test_bdd_save_2);
    tcase_add_test(tc_serialization, test_bdd_bad_save_1);
    tcase_add_test(tc_serialization, test_bdd_load_1);
    tcase_add_test(tc_serialization, test_bdd_load_2);
    suite_add_tcase(s, tc_serialization);

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
