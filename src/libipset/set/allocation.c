/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <glib.h>

#include <ipset/bdd/nodes.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


void
ipset_init(ip_set_t *set)
{
    /*
     * The set starts empty, so every value assignment should yield
     * false.
     */

    set->set_bdd = ipset_node_cache_terminal(ipset_cache, FALSE);
}


ip_set_t *
ipset_new()
{
    ip_set_t  *result = NULL;

    /*
     * Try to allocate a new set.
     */

    result = g_slice_new(ip_set_t);
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the set.
     */

    ipset_init(result);
    return result;
}


void
ipset_done(ip_set_t *set)
{
    /* nothing to do */
}


void
ipset_free(ip_set_t *set)
{
    ipset_done(set);
    g_slice_free(ip_set_t, set);
}
