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

#include <glib.h>
#include <gio/gio.h>

#include <ipset/bdd/nodes.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


gboolean
ipset_save(GOutputStream *stream,
           ip_set_t *set,
           GError **err)
{
    return ipset_node_cache_save
        (stream, ipset_cache, set->set_bdd, err);
}


gboolean
ipset_save_dot(GOutputStream *stream,
               ip_set_t *set,
               GError **err)
{
    return ipset_node_cache_save_dot
        (stream, ipset_cache, set->set_bdd, err);
}


ip_set_t *
ipset_load(GInputStream *stream,
           GError **err)
{
    ip_set_t  *set;
    ipset_node_id_t  node;

    set = ipset_new();
    if (set == NULL) return NULL;

    GError  *suberror = NULL;

    node = ipset_node_cache_load
        (stream, ipset_cache, &suberror);
    if (suberror != NULL)
    {
        g_propagate_error(err, suberror);
        ipset_free(set);
        return NULL;
    }

    set->set_bdd = node;
    return set;
}
