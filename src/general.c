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

int ipset_init()
{
    if (!bdd_isrunning())
    {
        int result = bdd_init(100000, 10000);
        if (result < 0)
            return result;

        ipset_ipv4_init_vars();
        ipset_ipv6_init_vars();

        return 0;
    } else {
        return 0;
    }
}
