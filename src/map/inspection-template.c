/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdbool.h>
#include <unistd.h>

#include <cudd.h>
#include <ipset/ipset.h>

bool
IPMAP_NAME(is_empty)(IP_MAP_T *map)
{
    /*
     * Since ADDs are unique, any map that maps all addresses to the
     * default value is “empty”.
     */

    return (map->map_add == map->default_add);
}

bool
IPMAP_NAME(is_equal)(IP_MAP_T *map1, IP_MAP_T *map2)
{
    /*
     * Since ADDs are unique, maps can only be equal if their ADDs are
     * equal.
     */

    return (map1->map_add == map2->map_add);
}

bool
IPMAP_NAME(is_not_equal)(IP_MAP_T *map1, IP_MAP_T *map2)
{
    /*
     * Since ADDs are unique, maps can only be equal if their ADDs are
     * equal.
     */

    return (map1->map_add != map2->map_add);
}

size_t
IPMAP_NAME(memory_size)(IP_MAP_T *map)
{
    return sizeof(DdNode) * Cudd_DagSize(map->map_add);
}


intptr_t
IPMAP_NAME(get)(IP_MAP_T *map, void *elem)
{
    DdNode  *elem_add;
    DdNode  *result_add;

    /*
     * First, construct the ADD that represents this IP address —
     * i.e., where each boolean variable is assigned TRUE or FALSE
     * depending on whether the corresponding bit is set in the
     * address.
     */

    elem_add = IPMAP_NAME(make_ip_add)(elem, IP_BIT_SIZE);

    /*
     * Then, evaluate the map's ADD with the elem ADD to get the
     * resulting value.
     */

    result_add = Cudd_addEvalConst(ipset_manager, elem_add,
                                   map->map_add);

    /*
     * We don't have to reference or derefernce the result ADD, since
     * we're just going to immediately return the result.
     */

    return Cudd_V(result_add);
}


void *
IPMAP_NAME(get_ptr)(IP_MAP_T *map, void *elem)
{
    return (void *) IPMAP_NAME(get_ptr)(map, elem);
}
