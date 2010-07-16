/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <glib.h>

#include <ipset/bdd/nodes.h>
#include <ipset/logging.h>


gsize
ipset_node_reachable_count(ipset_node_id_t node)
{
    /*
     * Create a set to track when we've visited a given node.
     */

    GHashTable  *visited = g_hash_table_new(NULL, NULL);

    /*
     * And a queue of nodes to check.
     */

    GQueue  queue = G_QUEUE_INIT;

    if (ipset_node_get_type(node) == IPSET_NONTERMINAL_NODE)
    {
        g_d_debug("Adding node %p to queue", node);
        g_queue_push_tail(&queue, node);
    }

    /*
     * And somewhere to store the result.
     */

    gsize  node_count = 0;

    /*
     * Check each node in turn.
     */

    while (!g_queue_is_empty(&queue))
    {
        ipset_node_id_t  curr = g_queue_pop_tail(&queue);

        /*
         * We don't have to do anything if this node is already in the
         * visited set.
         */

        if (!g_hash_table_lookup_extended(visited, curr, NULL, NULL))
        {
            g_d_debug("Visiting node %p for the first time", curr);

            /*
             * Add the node to the visited set.
             */

            g_hash_table_insert(visited, curr, NULL);

            /*
             * Increase the node count.
             */

            node_count++;

            /*
             * And add the node's nonterminal children to the visit
             * queue.
             */

            ipset_node_t  *node = ipset_nonterminal_node(curr);

            if (ipset_node_get_type(node->low) ==
                IPSET_NONTERMINAL_NODE)
            {
                g_d_debug("Adding node %p to queue", node->low);
                g_queue_push_tail(&queue, node->low);
            }

            if (ipset_node_get_type(node->high) ==
                IPSET_NONTERMINAL_NODE)
            {
                g_d_debug("Adding node %p to queue", node->high);
                g_queue_push_tail(&queue, node->high);
            }
        }
    }

    /*
     * Return the result, freeing everything before we go.
     */

    g_hash_table_destroy(visited);
    return node_count;
}


gsize
ipset_node_memory_size(ipset_node_id_t node)
{
    return ipset_node_reachable_count(node) * sizeof(ipset_node_t);
}
