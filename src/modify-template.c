/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <bdd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


bool
IPSET_NAME(add)(IP_SET_T *set, void *elem)
{
    BDD  elem_bdd;
    BDD  new_set_bdd;
    bool  elem_already_present;

    /*
     * First, construct the BDD that represents this IP address —
     * i.e., where each boolean variable is assigned TRUE or FALSE
     * depending on whether the corresponding bit is set in the
     * address.
     */

    elem_bdd = IPSET_NAME(make_ip_bdd)(elem, IP_BIT_SIZE);

    /*
     * Add elem to the set by constructing the logical OR of the old
     * set and the new element's BDD.
     */

    new_set_bdd = bdd_addref(bdd_or(set->set_bdd, elem_bdd));

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

    bdd_delref(set->set_bdd);
    bdd_delref(elem_bdd);
    set->set_bdd = new_set_bdd;

    /*
     * And return...
     */

    return elem_already_present;
}
