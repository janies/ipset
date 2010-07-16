/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>
#include <string.h>

#include <glib.h>

#include <ipset/bdd/nodes.h>
#include <ipset/logging.h>
#include "../hash.c.in"


GQuark
ipset_error_quark()
{
    return g_quark_from_static_string("ipset-error-quark");
}


ipset_node_type_t
ipset_node_get_type(ipset_node_id_t node)
{
    /*
     * The ID of a terminal node has its LSB set to 1, and has the
     * terminal value stored in the remaining bits.  The ID of a
     * nonterminal node is simply a pointer to the node struct.
     */

    guint  node_int = GPOINTER_TO_UINT(node);

    if ((node_int & 1) == 1)
    {
        return IPSET_TERMINAL_NODE;
    } else {
        return IPSET_NONTERMINAL_NODE;
    }
}


ipset_range_t
ipset_terminal_value(ipset_node_id_t node_id)
{
    /*
     * The ID of a terminal node has its LSB set to 1, and has the
     * terminal value stored in the remaining bits.
     */

    guint  node_int = GPOINTER_TO_UINT(node_id);
    return (gint) (node_int >> 1);
}


ipset_node_t *
ipset_nonterminal_node(ipset_node_id_t node_id)
{
    /*
     * The ID of a nonterminal node is simply a pointer to the node
     * struct.
     */

    return (ipset_node_t *) node_id;
}


void ipset_node_fprint(FILE *stream, ipset_node_t *node)
{
    fprintf(stream, "nonterminal(%u,%p,%p)",
            node->variable, node->low, node->high);
}


guint
ipset_node_hash(ipset_node_t *node)
{
    guint  hash = 0;
    combine_hash(&hash, node->variable);
    combine_hash(&hash, g_direct_hash(node->low));
    combine_hash(&hash, g_direct_hash(node->high));
    return hash;
}


gboolean
ipset_node_equal(const ipset_node_t *node1,
                 const ipset_node_t *node2)
{
    if (node1 == node2)
        return TRUE;

    return
        (node1->variable == node2->variable) &&
        (node1->low == node2->low) &&
        (node1->high == node2->high);
}


ipset_node_cache_t *
ipset_node_cache_new()
{
    ipset_node_cache_t  *cache;

    cache = g_slice_new(ipset_node_cache_t);
    cache->node_cache =
        g_hash_table_new((GHashFunc) ipset_node_hash,
                         (GEqualFunc) ipset_node_equal);

    cache->and_cache =
        g_hash_table_new((GHashFunc) ipset_binary_key_hash,
                         (GEqualFunc) ipset_binary_key_equal);

    cache->or_cache =
        g_hash_table_new((GHashFunc) ipset_binary_key_hash,
                         (GEqualFunc) ipset_binary_key_equal);

    return cache;
}


void
ipset_node_cache_free(ipset_node_cache_t *cache)
{
    g_hash_table_destroy(cache->node_cache);
    g_hash_table_destroy(cache->and_cache);
    g_hash_table_destroy(cache->or_cache);
    g_slice_free(ipset_node_cache_t, cache);
}


ipset_node_id_t
ipset_node_cache_terminal(ipset_node_cache_t *cache,
                          ipset_range_t value)
{
    /*
     * The ID of a terminal node has its LSB set to 1, and has the
     * terminal value stored in the remaining bits.
     */

    g_d_debug("Creating terminal node for %d", value);

    guint  node_int = (guint) value;
    node_int <<= 1;
    node_int |= 1;

    g_d_debug("Node ID is %p", GUINT_TO_POINTER(node_int));

    return GUINT_TO_POINTER(node_int);
}


ipset_node_id_t
ipset_node_cache_nonterminal(ipset_node_cache_t *cache,
                             ipset_variable_t variable,
                             ipset_node_id_t low,
                             ipset_node_id_t high)
{
    /*
     * Don't allow any nonterminals whose low and high subtrees are
     * the same, since the nonterminal would be redundant.
     */

    if (G_UNLIKELY(low == high))
    {
        g_d_debug("Skipping nonterminal(%u,%p,%p)",
                  variable, low, high);
        return low;
    }

    /*
     * Check to see if there's already a nonterminal with these
     * contents in the cache.
     */

    g_d_debug("Searching for nonterminal(%u,%p,%p)",
              variable, low, high);

    ipset_node_t  search_node;
    search_node.variable = variable;
    search_node.low = low;
    search_node.high = high;

    gpointer  found_node;
    gboolean  node_exists =
        g_hash_table_lookup_extended(cache->node_cache,
                                     &search_node,
                                     &found_node,
                                     NULL);

    if (node_exists)
    {
        /*
         * There's already a node with these contents, so return its
         * ID.
         */

        g_d_debug("Existing node, ID = %p", found_node);
        return found_node;
    } else {
        /*
         * This node doesn't exist yet.  Allocate a permanent copy of
         * the node, add it to the cache, and then return its ID.
         */

        ipset_node_t  *real_node = g_slice_new(ipset_node_t);
        memcpy(real_node, &search_node, sizeof(ipset_node_t));

        g_hash_table_insert(cache->node_cache, real_node, NULL);

        g_d_debug("NEW node, ID = %p", real_node);
        return real_node;
    }
}


gboolean
ipset_bool_array_assignment(gconstpointer user_data,
                            ipset_variable_t variable)
{
    const gboolean  *bool_array = (const gboolean *) user_data;
    return bool_array[variable];
}


gboolean
ipset_bit_array_assignment(gconstpointer user_data,
                           ipset_variable_t variable)
{
    const guint8  *bit_array = (const guint8 *) user_data;
    unsigned int  byte_index = variable / 8;
    unsigned int  bit_number = variable % 8;
    unsigned int  bit_mask = 0x80 >> bit_number;

    return ((bit_array[byte_index] & bit_mask) != 0);
}


ipset_range_t
ipset_node_evaluate(ipset_node_id_t node_id,
                    ipset_assignment_func_t assignment,
                    gconstpointer user_data)
{
    ipset_node_id_t  curr_node_id = node_id;

    g_d_debug("Evaluating BDD node %p", node_id);

    /*
     * As long as the current node is a nonterminal, we have to check
     * the value of the current variable.
     */

    while (ipset_node_get_type(curr_node_id) == IPSET_NONTERMINAL_NODE)
    {
        /*
         * We have to look up this variable in the assignment.
         */

        ipset_node_t  *node = ipset_nonterminal_node(curr_node_id);
        gboolean  this_value = assignment(user_data, node->variable);

        g_d_debug("Variable %u has value %s", node->variable,
                  this_value? "TRUE": "FALSE");

        if (this_value)
        {
            /*
             * This node's variable is true in the assignment vector,
             * so trace down the high subtree.
             */

            curr_node_id = node->high;
        } else {
            /*
             * This node's variable is false in the assignment vector,
             * so trace down the low subtree.
             */

            curr_node_id = node->low;
        }
    }

    /*
     * Once we find a terminal node, we've got the final result.
     */

    g_d_debug("Evaluated result is %d",
              ipset_terminal_value(curr_node_id));

    return ipset_terminal_value(curr_node_id);
}
