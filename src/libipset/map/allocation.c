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


void
ipmap_init(ip_map_t *map, gint default_value)
{
    /*
     * The map starts empty, so every value assignment should yield
     * the default.
     */

    map->default_bdd =
        ipset_node_cache_terminal(ipset_cache, default_value);

    map->map_bdd = map->default_bdd;
}


ip_map_t *
ipmap_new(gint default_value)
{
    ip_map_t  *result = NULL;

    /*
     * Try to allocate a new map.
     */

    result = g_slice_new(ip_map_t);
    if (result == NULL)
        return NULL;

    /*
     * If that worked, initialize and return the map.
     */

    ipmap_init(result, default_value);
    return result;
}


void
ipmap_done(ip_map_t *map)
{
    /* nothing to do */
}


void
ipmap_free(ip_map_t *map)
{
    ipmap_done(map);
    g_slice_free(ip_map_t, map);
}
