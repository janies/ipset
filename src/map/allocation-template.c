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

#include <cudd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


void
IPMAP_NAME(init)(IP_MAP_T *map, intptr_t default_value)
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


IP_MAP_T *
IPMAP_NAME(new)(intptr_t default_value)
{
    IP_MAP_T  *result = NULL;

    /*
     * Try to allocate a new map.
     */

    result = (IP_MAP_T *) malloc(sizeof(IP_MAP_T));
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the map.
     */

    IPMAP_NAME(init)(result, default_value);
    return result;
}


void
IPMAP_NAME(init_ptr)(IP_MAP_T *map, void *default_value)
{
    return IPMAP_NAME(init)(map, (intptr_t) default_value);
}


IP_MAP_T *
IPMAP_NAME(new_ptr)(void *default_value)
{
    return IPMAP_NAME(new)((intptr_t) default_value);
}


void
IPMAP_NAME(done)(IP_MAP_T *map)
{
    Cudd_RecursiveDeref(ipset_manager, map->map_add);
    Cudd_RecursiveDeref(ipset_manager, map->default_add);
}


void
IPMAP_NAME(free)(IP_MAP_T *map)
{
    IPMAP_NAME(done)(map);
    free(map);
}
