/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>

#include <cudd.h>
#include <dddmp.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


bool
ipset_save(ip_set_t *set, FILE *file)
{
    return
        Dddmp_cuddBddStore(ipset_manager, NULL, set->set_bdd,
                           NULL, NULL,
                           DDDMP_MODE_TEXT,
                           DDDMP_VARIDS,
                           NULL, file);
}


ip_set_t *
ipset_load(FILE *file)
{
    ip_set_t  *set;
    DdNode  *node;

    set = ipset_new();
    if (set == NULL) return NULL;

    node =
        Dddmp_cuddBddLoad(ipset_manager,
                          DDDMP_VAR_MATCHIDS,
                          NULL, NULL, NULL,
                          DDDMP_MODE_TEXT,
                          NULL, file);

    if (node == NULL)
    {
        ipset_free(set);
        return NULL;
    }

    set->set_bdd = node;
    return set;
}
