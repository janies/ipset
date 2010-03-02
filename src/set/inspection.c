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
#include <stdio.h>
#include <unistd.h>

#include <cudd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>

bool
ipset_is_empty(ip_set_t *set)
{
    /*
     * Since BDDs are unique, the only empty set is the “false” BDD.
     */

    return (set->set_bdd == Cudd_ReadLogicZero(ipset_manager));
}

bool
ipset_is_equal(ip_set_t *set1, ip_set_t *set2)
{
    /*
     * Since BDDs are unique, sets can only be equal if their BDDs are
     * equal.
     */

    return (set1->set_bdd == set2->set_bdd);
}

bool
ipset_is_not_equal(ip_set_t *set1, ip_set_t *set2)
{
    /*
     * Since BDDs are unique, sets can only be equal if their BDDs are
     * equal.
     */

    return (set1->set_bdd != set2->set_bdd);
}

size_t
ipset_memory_size(ip_set_t *set)
{
    return sizeof(DdNode) * Cudd_DagSize(set->set_bdd);
}
