/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <glib.h>

#include <ipset/bdd/nodes.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


ipset_node_cache_t *
ipset_cache = NULL;


int
ipset_init_library()
{
    if (G_UNLIKELY(ipset_cache == NULL))
    {
        ipset_cache = ipset_node_cache_new();

        if (ipset_cache == NULL)
            return 1;

        return 0;
    } else {
        return 0;
    }
}
