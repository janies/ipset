/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <glib.h>

#include <ipset/bdd/nodes.h>
#include "../hash.c.in"


guint
ipset_binary_key_hash(ipset_binary_key_t *key)
{
    guint  hash = 0;
    combine_hash(&hash, g_direct_hash(key->lhs));
    combine_hash(&hash, g_direct_hash(key->rhs));
    return hash;
}


gboolean
ipset_binary_key_equal(const ipset_binary_key_t *key1,
                       const ipset_binary_key_t *key2)
{
    if (key1 == key2)
        return TRUE;

    return
        (key1->lhs == key2->lhs) &&
        (key1->rhs == key2->rhs);
}


void
ipset_binary_key_commutative(ipset_binary_key_t *key,
                             ipset_node_id_t lhs,
                             ipset_node_id_t rhs)
{
    /*
     * Since the operator is commutative, make sure that the LHS is
     * smaller than the RHS.
     */

    if (lhs < rhs)
    {
        key->lhs = lhs;
        key->rhs = rhs;
    } else {
        key->lhs = rhs;
        key->rhs = lhs;
    }
}


/**
 * Recurse down one subtree (the LHS).
 */

static ipset_node_id_t
recurse_left(ipset_node_cache_t *cache,
             ipset_node_t *lhs_node,
             ipset_node_id_t rhs)
{
    ipset_node_id_t  result_low =
        ipset_node_cache_and(cache, lhs_node->low, rhs);
    ipset_node_id_t  result_high =
        ipset_node_cache_and(cache, lhs_node->high, rhs);

    return ipset_node_cache_nonterminal
        (cache, lhs_node->variable, result_low, result_high);
}


/**
 * Recurse down both subtrees simultaneously.
 */

static ipset_node_id_t
recurse_both(ipset_node_cache_t *cache,
             ipset_node_t *lhs_node,
             ipset_node_t *rhs_node)
{
    ipset_node_id_t  result_low =
        ipset_node_cache_and(cache, lhs_node->low, rhs_node->low);
    ipset_node_id_t  result_high =
        ipset_node_cache_and(cache, lhs_node->high, rhs_node->high);

    return ipset_node_cache_nonterminal
        (cache, lhs_node->variable, result_low, result_high);
}


/**
 * Perform an actual AND operation.
 */

static ipset_node_id_t
apply_and(ipset_node_cache_t *cache,
          ipset_node_id_t lhs,
          ipset_node_id_t rhs)
{
    if (ipset_node_get_type(lhs) == IPSET_TERMINAL_NODE)
    {
        if (ipset_node_get_type(rhs) == IPSET_TERMINAL_NODE)
        {
            /*
             * When both nodes are terminal, we apply the operator to
             * the terminals' values, and construct a new terminal
             * from the result.  Note that we do not verify that the
             * operator returns a positive value.
             */

            ipset_range_t  lhs_value = ipset_terminal_value(lhs);
            ipset_range_t  rhs_value = ipset_terminal_value(rhs);
            ipset_range_t  new_value = lhs_value & rhs_value;

            return ipset_node_cache_terminal(cache, new_value);
        } else {
            /*
             * When one node is terminal, and the other is
             * nonterminal, we recurse down the subtrees of the
             * nonterminal, combining the results with the terminal.
             */

            ipset_node_t  *rhs_node = ipset_nonterminal_node(rhs);
            return recurse_left(cache, rhs_node, lhs);
        }
    } else {
        if (ipset_node_get_type(rhs) == IPSET_TERMINAL_NODE)
        {
            /*
             * When one node is terminal, and the other is
             * nonterminal, we recurse down the subtrees of the
             * nonterminal, combining the results with the terminal.
             */

            ipset_node_t  *lhs_node = ipset_nonterminal_node(lhs);
            return recurse_left(cache, lhs_node, rhs);
        } else {
            /*
             * When both nodes are nonterminal, the way we recurse
             * depends on the variables of the nonterminals.  We
             * always recurse down the nonterminal with the smaller
             * variable index.  This ensures that our BDDs remain
             * ordered.
             */

            ipset_node_t  *lhs_node = ipset_nonterminal_node(lhs);
            ipset_node_t  *rhs_node = ipset_nonterminal_node(rhs);

            if (lhs_node->variable == rhs_node->variable)
            {
                return recurse_both(cache, lhs_node, rhs_node);
            } else if (lhs_node->variable < rhs_node->variable) {
                return recurse_left(cache, lhs_node, rhs);
            } else {
                return recurse_left(cache, rhs_node, lhs);
            }
        }
    }
}


ipset_node_id_t
ipset_node_cache_and(ipset_node_cache_t *cache,
                     ipset_node_id_t lhs,
                     ipset_node_id_t rhs)
{
    /*
     * Check to see if we've already performed the operation on these
     * operands.
     */

    g_debug("Applying AND(%p, %p)", lhs, rhs);

    ipset_binary_key_t  search_key;
    ipset_binary_key_commutative(&search_key, lhs, rhs);

    gpointer  found_key;
    gpointer  found_result;
    gboolean  node_exists =
        g_hash_table_lookup_extended(cache->and_cache,
                                     &search_key,
                                     &found_key,
                                     &found_result);

    if (node_exists)
    {
        /*
         * There's a result in the cache, so return it.
         */

        g_debug("Existing result = %p", found_result);
        return found_result;
    } else {
        /*
         * This result doesn't exist yet.  Allocate a permanent copy
         * of the key.  Apply the operator, add the result to the
         * cache, and then return it.
         */

        ipset_binary_key_t  *real_key =
            g_slice_new(ipset_binary_key_t);
        memcpy(real_key, &search_key, sizeof(ipset_binary_key_t));

        ipset_node_id_t  result = apply_and(cache, lhs, rhs);
        g_debug("NEW result = %p", result);

        g_hash_table_insert(cache->and_cache, real_key, result);
        return result;
    }
}
