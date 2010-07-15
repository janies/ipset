/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
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


/**
 * The number of ADDs we have to store/load when reading and writing
 * an IP map.
 */

#define NUM_ADDS 2


bool
ipmap_save(ip_map_t *map, FILE *file)
{
    DdNode  *nodes[NUM_ADDS];

    nodes[0] = map->map_add;
    nodes[1] = map->default_add;

    return
        Dddmp_cuddAddArrayStore(ipset_manager, NULL,
                                NUM_ADDS, nodes,
                                NULL, NULL, NULL,
                                DDDMP_MODE_TEXT,
                                DDDMP_VARIDS,
                                NULL, file);
}


ip_map_t *
ipmap_load(FILE *file)
{
    ip_map_t  *map;
    DdNode  **nodes;
    int  num_roots;

    /*
     * It doesn't matter what default value we use here, because we're
     * going to replace it with the default ADD we load in from the
     * file.
     */

    map = ipmap_new(0);
    if (map == NULL) return NULL;

    num_roots =
        Dddmp_cuddAddArrayLoad(ipset_manager,
                               DDDMP_ROOT_MATCHLIST,
                               NULL,
                               DDDMP_VAR_MATCHIDS,
                               NULL, NULL, NULL,
                               DDDMP_MODE_TEXT,
                               NULL, file,
                               &nodes);

    if (num_roots != 2)
    {
        if (nodes != NULL)
            free(nodes);

        ipmap_free(map);
        return NULL;
    }

    map->map_add = nodes[0];
    map->default_add = nodes[1];
    return map;
}
