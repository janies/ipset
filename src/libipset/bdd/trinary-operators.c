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
#include <ipset/logging.h>
#include "../hash.c.in"


guint
ipset_trinary_key_hash(ipset_trinary_key_t *key)
{
    guint  hash = 0;
    combine_hash(&hash, g_direct_hash(key->f));
    combine_hash(&hash, g_direct_hash(key->g));
    combine_hash(&hash, g_direct_hash(key->h));
    return hash;
}


gboolean
ipset_trinary_key_equal(const ipset_trinary_key_t *key1,
                        const ipset_trinary_key_t *key2)
{
    if (key1 == key2)
        return TRUE;

    return
        (key1->f == key2->f) &&
        (key1->g == key2->g) &&
        (key1->h == key2->h);
}


void
ipset_trinary_key_init(ipset_trinary_key_t *key,
                       ipset_node_id_t f,
                       ipset_node_id_t g,
                       ipset_node_id_t h)
{
    key->f = f;
    key->g = g;
    key->h = h;
}


// forward declaration

static ipset_node_id_t
cached_ite(ipset_node_cache_t *cache,
           ipset_node_id_t f,
           ipset_node_id_t g,
           ipset_node_id_t h);


/**
 * Perform an actual trinary operation.
 */

static ipset_node_id_t
apply_ite(ipset_node_cache_t *cache,
          ipset_node_id_t f,
          ipset_node_id_t g,
          ipset_node_id_t h)
{
    /*
     * We know this isn't a trivial case, since otherwise it wouldn't
     * been picked up in cached_ite(), so we need to recurse.
     */

    g_assert(ipset_node_get_type(f) == IPSET_NONTERMINAL_NODE);

    ipset_node_t  *f_node = ipset_nonterminal_node(f);
    ipset_node_t  *g_node = NULL;
    ipset_node_t  *h_node = NULL;

    /*
     * There's at least one nonterminal node.  We need the lowest
     * nonterminal variable index.
     */

    ipset_variable_t  min_variable = f_node->variable;

    if (ipset_node_get_type(g) == IPSET_NONTERMINAL_NODE)
    {
        g_node = ipset_nonterminal_node(g);

        if (g_node->variable < min_variable)
        {
            min_variable = g_node->variable;
        }
    }

    if (ipset_node_get_type(h) == IPSET_NONTERMINAL_NODE)
    {
        h_node = ipset_nonterminal_node(h);

        if (h_node->variable < min_variable)
        {
            min_variable = h_node->variable;
        }
    }

    /*
     * We're going to do two recursive calls, a “low” one and a “high”
     * one.  For each nonterminal that has the minimum variable
     * number, we use its low and high pointers in the respective
     * recursive call.  For all other nonterminals, and for all
     * terminals, we use the operand itself.
     */

    ipset_node_id_t  low_f, high_f;
    ipset_node_id_t  low_g, high_g;
    ipset_node_id_t  low_h, high_h;

    /* we know that F is nonterminal */
    if (f_node->variable == min_variable)
    {
        low_f = f_node->low;
        high_f = f_node->high;
    } else {
        low_f = f;
        high_f = f;
    }

    if ((ipset_node_get_type(g) == IPSET_NONTERMINAL_NODE) &&
        (g_node->variable == min_variable))
    {
        low_g = g_node->low;
        high_g = g_node->high;
    } else {
        low_g = g;
        high_g = g;
    }

    if ((ipset_node_get_type(h) == IPSET_NONTERMINAL_NODE) &&
        (h_node->variable == min_variable))
    {
        low_h = h_node->low;
        high_h = h_node->high;
    } else {
        low_h = h;
        high_h = h;
    }

    /*
     * Perform the recursion.
     */

    ipset_node_id_t low_result =
        cached_ite(cache, low_f, low_g, low_h);
    ipset_node_id_t high_result =
        cached_ite(cache, high_f, high_g, high_h);

    return ipset_node_cache_nonterminal
        (cache, min_variable, low_result, high_result);
}


/**
 * Perform an actual trinary operation, checking the cache first.
 */

static ipset_node_id_t
cached_ite(ipset_node_cache_t *cache,
           ipset_node_id_t f,
           ipset_node_id_t g,
           ipset_node_id_t h)
{
    g_d_debug("Applying ITE(%p,%p,%p)", f, g, h);

    /*
     * Some trivial cases first.
     */

    /*
     * If F is a terminal, then we're in one of the following two
     * cases:
     *
     *   ITE(1,G,H) = G
     *   ITE(0,G,H) = H
     */

    if (ipset_node_get_type(f) == IPSET_TERMINAL_NODE)
    {
        ipset_range_t  f_value = ipset_terminal_value(f);
        ipset_node_id_t  result = (f_value == 0)? h: g;
        g_d_debug("Trivial result = %p", result);
        return result;
    }

    /*
     * ITE(F,G,G) == G
     */

    if (g == h)
    {
        g_d_debug("Trivial result = %p", g);
        return g;
    }

    /*
     * ITE(F,1,0) = F
     */

    if ((ipset_node_get_type(g) == IPSET_TERMINAL_NODE) &&
        (ipset_node_get_type(h) == IPSET_TERMINAL_NODE))
    {
        ipset_range_t  g_value = ipset_terminal_value(g);
        ipset_range_t  h_value = ipset_terminal_value(h);

        if ((g_value == 1) && (h_value == 0))
        {
            g_d_debug("Trivial result = %p", f);
            return f;
        }
    }

    /*
     * Check to see if we've already performed the operation on these
     * operands.
     */

    ipset_trinary_key_t  search_key;
    ipset_trinary_key_init(&search_key, f, g, h);

    gpointer  found_key;
    gpointer  found_result;
    gboolean  node_exists =
        g_hash_table_lookup_extended(cache->ite_cache,
                                     &search_key,
                                     &found_key,
                                     &found_result);

    if (node_exists)
    {
        /*
         * There's a result in the cache, so return it.
         */

        g_d_debug("Existing result = %p", found_result);
        return found_result;
    } else {
        /*
         * This result doesn't exist yet.  Allocate a permanent copy
         * of the key.  Apply the operator, add the result to the
         * cache, and then return it.
         */

        ipset_trinary_key_t  *real_key =
            g_slice_new(ipset_trinary_key_t);
        memcpy(real_key, &search_key, sizeof(ipset_trinary_key_t));

        ipset_node_id_t  result =
            apply_ite(cache, f, g, h);
        g_d_debug("NEW result = %p", result);

        g_hash_table_insert(cache->ite_cache, real_key, result);
        return result;
    }
}


ipset_node_id_t
ipset_node_cache_ite(ipset_node_cache_t *cache,
                     ipset_node_id_t f,
                     ipset_node_id_t g,
                     ipset_node_id_t h)
{
    return cached_ite(cache, f, g, h);
}
