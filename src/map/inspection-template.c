/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>

#include <cudd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


intptr_t
IPMAP_NAME(get)(ip_map_t *map, void *elem)
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
IPMAP_NAME(get_ptr)(ip_map_t *map, void *elem)
{
    return (void *) IPMAP_NAME(get_ptr)(map, elem);
}
