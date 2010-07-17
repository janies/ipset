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

gboolean
ipset_is_empty(ip_set_t *set)
{
    /*
     * Since BDDs are unique, the only empty set is the “false” BDD.
     */

    return (set->set_bdd ==
            ipset_node_cache_terminal(ipset_cache, FALSE));
}

gboolean
ipset_is_equal(ip_set_t *set1, ip_set_t *set2)
{
    /*
     * Since BDDs are unique, sets can only be equal if their BDDs are
     * equal.
     */

    return (set1->set_bdd == set2->set_bdd);
}

gboolean
ipset_is_not_equal(ip_set_t *set1, ip_set_t *set2)
{
    /*
     * Since BDDs are unique, sets can only be equal if their BDDs are
     * equal.
     */

    return (set1->set_bdd != set2->set_bdd);
}

gsize
ipset_memory_size(ip_set_t *set)
{
    return ipset_node_memory_size(set->set_bdd);
}
