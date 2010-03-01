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
        ipset_manager = Cudd_Init(IPV4_BIT_SIZE + IPV6_BIT_SIZE, 0,
                                  CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS,
                                  0);

        if (ipset_manager == NULL)
            return 1;

        ipset_ipv4_init_vars();
        ipset_ipv6_init_vars();

        return 0;
    } else {
        return 0;
    }
}
