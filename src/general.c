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

#include <cudd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>

DdManager *
ipset_manager = NULL;

int ipset_init()
{
    if (ipset_manager == NULL)
    {
        ipset_manager = Cudd_Init(0, 0,
                                  CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS,
                                  0);

        if (ipset_manager == NULL)
            return 1;

        ipmap_ipv4_init_add_vars();
        ipmap_ipv6_init_add_vars();

        ipset_ipv4_init_bdd_vars();
        ipset_ipv6_init_bdd_vars();

        return 0;
    } else {
        return 0;
    }
}
