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
IPSET_NAME(init)(IP_SET_T *set)
{
    /*
     * The set starts empty, so every value assignment should yield
     * false.
     */

    set->set_bdd = bdd_addref(bddfalse);
}


IP_SET_T *
IPSET_NAME(new)()
{
    IP_SET_T  *result = NULL;

    /*
     * Try to allocate a new set.
     */

    result = (IP_SET_T *) malloc(sizeof(IP_SET_T));
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the set.
     */

    IPSET_NAME(init)(result);
    return result;
}


void
IPSET_NAME(done)(IP_SET_T *set)
{
    bdd_delref(set->set_bdd);
}


void
IPSET_NAME(free)(IP_SET_T *set)
{
    IPSET_NAME(done)(set);
    free(set);
}
