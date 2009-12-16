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

#include <bdd.h>
#include <ipset/ipset.h>


void
ipset_init_ipv4_set(ipv4_set_t *set)
{
    /*
     * The set starts empty, so every value assignment should yield
     * false.
     */

    set->set_bdd = bdd_addref(bddfalse);
}


ipv4_set_t *
ipset_new_ipv4_set()
{
    ipv4_set_t  *result = NULL;

    /*
     * Try to allocate a new set.
     */

    result = (ipv4_set_t *) malloc(sizeof(ipv4_set_t));
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the set.
     */

    ipset_init_ipv4_set(result);
    return result;
}


void
ipset_done_ipv4_set(ipv4_set_t *set)
{
    bdd_delref(set->set_bdd);
}


void
ipset_free_ipv4_set(ipv4_set_t *set)
{
    ipset_done_ipv4_set(set);
    free(set);
}
