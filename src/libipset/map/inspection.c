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
ipmap_is_empty(ip_map_t *map)
{
    /*
     * Since BDDs are unique, any map that maps all addresses to the
     * default value is “empty”.
     */

    return (map->map_bdd == map->default_bdd);
}

gboolean
ipmap_is_equal(ip_map_t *map1, ip_map_t *map2)
{
    /*
     * Since BDDs are unique, maps can only be equal if their BDDs are
     * equal.
     */

    return (map1->map_bdd == map2->map_bdd);
}

gboolean
ipmap_is_not_equal(ip_map_t *map1, ip_map_t *map2)
{
    /*
     * Since BDDs are unique, maps can only be equal if their BDDs are
     * equal.
     */

    return (map1->map_bdd != map2->map_bdd);
}

gsize
ipmap_memory_size(ip_map_t *map)
{
    return ipset_node_memory_size(map->map_bdd);
}
