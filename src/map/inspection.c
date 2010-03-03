/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <cudd.h>
#include <ipset/ipset.h>

bool
ipmap_is_empty(ip_map_t *map)
{
    /*
     * Since ADDs are unique, any map that maps all addresses to the
     * default value is “empty”.
     */

    return (map->map_add == map->default_add);
}

bool
ipmap_is_equal(ip_map_t *map1, ip_map_t *map2)
{
    /*
     * Since ADDs are unique, maps can only be equal if their ADDs are
     * equal.
     */

    return (map1->map_add == map2->map_add);
}

bool
ipmap_is_not_equal(ip_map_t *map1, ip_map_t *map2)
{
    /*
     * Since ADDs are unique, maps can only be equal if their ADDs are
     * equal.
     */

    return (map1->map_add != map2->map_add);
}

size_t
ipmap_memory_size(ip_map_t *map)
{
    return sizeof(DdNode) * Cudd_DagSize(map->map_add);
}
