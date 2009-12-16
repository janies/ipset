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


int ipset_init()
{
    if (!bdd_isrunning())
    {
        return bdd_init(100000, 10000);
    } else {
        return 0;
    }
}
