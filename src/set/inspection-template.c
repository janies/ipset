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
IPSET_NAME(is_empty)(IP_SET_T *set)
{
    /*
     * Since BDDs are unique, the only empty set is the “false” BDD.
     */

    return (set->set_bdd == Cudd_ReadLogicZero(ipset_manager));
}

bool
IPSET_NAME(is_equal)(IP_SET_T *set1, IP_SET_T *set2)
{
    /*
     * Since BDDs are unique, sets can only be equal if their BDDs are
     * equal.
     */

    return (set1->set_bdd == set2->set_bdd);
}

bool
IPSET_NAME(is_not_equal)(IP_SET_T *set1, IP_SET_T *set2)
{
    /*
     * Since BDDs are unique, sets can only be equal if their BDDs are
     * equal.
     */

    return (set1->set_bdd != set2->set_bdd);
}

size_t
IPSET_NAME(memory_size)(IP_SET_T *set)
{
    return sizeof(DdNode) * Cudd_DagSize(set->set_bdd);
}
