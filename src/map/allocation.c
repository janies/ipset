/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>

#include <cudd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


void
ipmap_init(ip_map_t *map, intptr_t default_value)
{
    /*
     * The map starts empty, so every value assignment should yield
     * false.
     */

    map->default_add =
        Cudd_addConst(ipset_manager, default_value);
    Cudd_Ref(map->default_add);

    map->map_add = map->default_add;
    Cudd_Ref(map->map_add);
}


ip_map_t *
ipmap_new(intptr_t default_value)
{
    ip_map_t  *result = NULL;

    /*
     * Try to allocate a new map.
     */

    result = (ip_map_t *) malloc(sizeof(ip_map_t));
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the map.
     */

    ipmap_init(result, default_value);
    return result;
}


void
ipmap_init_ptr(ip_map_t *map, void *default_value)
{
    return ipmap_init(map, (intptr_t) default_value);
}


ip_map_t *
ipmap_new_ptr(void *default_value)
{
    return ipmap_new((intptr_t) default_value);
}


void
ipmap_done(ip_map_t *map)
{
    Cudd_RecursiveDeref(ipset_manager, map->map_add);
    Cudd_RecursiveDeref(ipset_manager, map->default_add);
}


void
ipmap_free(ip_map_t *map)
{
    ipmap_done(map);
    free(map);
}
