/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
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
ipset_init(ip_set_t *set)
{
    /*
     * The set starts empty, so every value assignment should yield
     * false.
     */

    set->set_bdd = Cudd_ReadLogicZero(ipset_manager);
    Cudd_Ref(set->set_bdd);
}


ip_set_t *
ipset_new()
{
    ip_set_t  *result = NULL;

    /*
     * Try to allocate a new set.
     */

    result = (ip_set_t *) malloc(sizeof(ip_set_t));
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the set.
     */

    ipset_init(result);
    return result;
}


void
ipset_done(ip_set_t *set)
{
    Cudd_RecursiveDeref(ipset_manager, set->set_bdd);
}


void
ipset_free(ip_set_t *set)
{
    ipset_done(set);
    free(set);
}
