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


bool
IPSET_NAME(add_network)(ip_set_t *set, void *elem, int netmask)
{
    DdNode  *elem_bdd;
    DdNode  *new_set_bdd;
    bool  elem_already_present;

    /*
     * First, construct the BDD that represents this IP address —
     * i.e., where each boolean variable is assigned TRUE or FALSE
     * depending on whether the corresponding bit is set in the
     * address.
     */

    elem_bdd = IPSET_NAME(make_ip_bdd)(elem, netmask);

    /*
     * Add elem to the set by constructing the logical OR of the old
     * set and the new element's BDD.
     */

    new_set_bdd = Cudd_bddOr(ipset_manager, set->set_bdd, elem_bdd);
    Cudd_Ref(new_set_bdd);

    /*
     * If the BDD representing the set hasn't changed, then the
     * element was already in the set.
     */

    elem_already_present = (new_set_bdd == set->set_bdd);

    /*
     * Store the set's new BDD into the set struct.  Dereference the
     * old set and the element's BDD, since we don't need them
     * anymore.
     */

    Cudd_RecursiveDeref(ipset_manager, elem_bdd);
    Cudd_RecursiveDeref(ipset_manager, set->set_bdd);
    set->set_bdd = new_set_bdd;

    /*
     * And return...
     */

    return elem_already_present;
}


bool
IPSET_NAME(add)(ip_set_t *set, void *elem)
{
    return IPSET_NAME(add_network)(set, elem, IP_BIT_SIZE);
}
