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


/**
 * The number of BDDs we have to store/load when reading and writing
 * an IP map.
 */

#define NUM_BDDS 2


gboolean
ipmap_save(GOutputStream *stream,
           ip_map_t *map,
           GError **err)
{
    return ipset_node_cache_save
        (stream, ipset_cache, map->map_bdd, err);
}


ip_map_t *
ipmap_load(GInputStream *stream,
           GError **err)
{
    ip_map_t  *map;
    ipset_node_id_t  node;

    /*
     * It doesn't matter what default value we use here, because we're
     * going to replace it with the default BDD we load in from the
     * file.
     */

    map = ipmap_new(0);
    if (map == NULL) return NULL;

    GError  *suberror = NULL;

    node = ipset_node_cache_load
        (stream, ipset_cache, &suberror);
    if (suberror != NULL)
    {
        g_propagate_error(err, suberror);
        ipmap_free(map);
        return NULL;
    }

    map->map_bdd = node;
    return map;
}
