/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <cudd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


void
IPMAP_NAME(set_network)(IP_MAP_T *map,
                        void *elem,
                        int netmask,
                        intptr_t value)
{
    DdNode  *elem_add;
    DdNode  *value_add;
    DdNode  *new_map_add;

    /*
     * First, construct the ADD that represents this IP address —
     * i.e., where each boolean variable is assigned TRUE or FALSE
     * depending on whether the corresponding bit is set in the
     * address.
     */

    elem_add = IPMAP_NAME(make_ip_add)(elem, netmask);

    /*
     * Next, create a new constant ADD to represent the value.
     */

    value_add = Cudd_addConst(ipset_manager, value);
    Cudd_Ref(value_add);

    /*
     * Add elem to the map by constructing an if-then-else ADD.  If
     * the variables evaluate just like in elem_add (signifying that
     * the address is the same/in the same network), then we point to
     * the value_add to get the value of the function; otherwise we
     * point at the previous map's ADD.  This ensures that any
     * previous values that aren't overwritten by the new IP network
     * address keep the same value as before.
     */

    new_map_add = Cudd_addIte(ipset_manager, elem_add,
                              value_add, map->map_add);
    Cudd_Ref(new_map_add);

    /*
     * Store the map's new ADD into the map struct.  Dereference the
     * old map and the element's ADD, since we don't need them
     * anymore.
     */

    Cudd_RecursiveDeref(ipset_manager, elem_add);
    Cudd_RecursiveDeref(ipset_manager, value_add);
    Cudd_RecursiveDeref(ipset_manager, map->map_add);
    map->map_add = new_map_add;

    /*
     * And return...
     */

    return;
}


void
IPMAP_NAME(set)(IP_MAP_T *map, void *elem, intptr_t value)
{
    return IPMAP_NAME(set_network)(map, elem, IP_BIT_SIZE, value);
}


void
IPMAP_NAME(set_network_ptr)(IP_MAP_T *map,
                            void *elem,
                            int netmask,
                            void *value)
{
    return IPMAP_NAME(set_network)(map, elem, netmask,
                                   (intptr_t) value);
}


void
IPMAP_NAME(set_ptr)(IP_MAP_T *map, void *elem, void *value)
{
    return IPMAP_NAME(set)(map, elem, (intptr_t) value);
}
