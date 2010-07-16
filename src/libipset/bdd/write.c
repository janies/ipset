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
#include <gio/gio.h>

#include <ipset/bdd/nodes.h>
#include <ipset/logging.h>


static const char  MAGIC_NUMBER[] = "IP set";
static const gsize  MAGIC_NUMBER_LENGTH = 6;


/**
 * A helper method that checks the result of a Boolean expression, and
 * returns FALSE if the expression is FALSE.
 */

#define CHECK(expr)                             \
    G_STMT_START {                              \
        if (!(expr))                            \
        {                                       \
            return FALSE;                       \
        }                                       \
    } G_STMT_END


/**
 * On disk, we use a different node ID scheme than we do in memory.
 * Terminal node IDs are non-negative, and are equal to the terminal
 * value.  Nonterminal node IDs are negative, starting with -1.
 * Nonterminal -1 appears first on disk, then nonterminal -2, and so
 * on.
 */

typedef gint  serialized_id_t;



/**
 * A helper function for ipset_node_save().  Outputs a nonterminal
 * node in a BDD tree, if we haven't done so already.  Ensures that
 * the children of the nonterminal are output before the nonterminal
 * is.  Return the serialized ID of this node.
 */

serialized_id_t
save_visit_node(GDataOutputStream *dstream,
                GHashTable *serialized_ids,
                serialized_id_t *next_serialized_id,
                ipset_node_id_t node_id)
{
    /*
     * Check whether we've already serialized this node.
     */

    gpointer  serialized_ptr =
        g_hash_table_lookup(serialized_ids, node_id);
    serialized_id_t  serialized_id = GPOINTER_TO_INT(serialized_ptr);

    if (serialized_ptr == NULL)
    {
        ipset_node_t  *node = ipset_nonterminal_node(node_id);

        g_d_debug("Visiting node %p nonterminal(%u,%p,%p)",
                  node_id, node->variable, node->low, node->high);

        /*
         * Output the node's nonterminal children before we output the
         * node itself.
         */

        serialized_id_t  serialized_low;

        if (ipset_node_get_type(node->low) == IPSET_NONTERMINAL_NODE)
        {
            serialized_low =
                save_visit_node(dstream, serialized_ids,
                                next_serialized_id, node->low);
        } else {
            serialized_low =
                ipset_terminal_value(node->low);
        }

        serialized_id_t  serialized_high;

        if (ipset_node_get_type(node->high) == IPSET_NONTERMINAL_NODE)
        {
            serialized_high =
                save_visit_node(dstream, serialized_ids,
                                next_serialized_id, node->high);
        } else {
            serialized_high =
                ipset_terminal_value(node->high);
        }

        /*
         * Output the current node
         */

        serialized_id = (*next_serialized_id)--;
        g_d_debug("Outputing node %p as serialized node %d"
                  " = (%u,%d,%d)",
                  node_id, serialized_id,
                  node->variable, serialized_low, serialized_high);

        CHECK(g_data_output_stream_put_byte
              (dstream, node->variable, NULL, NULL));
        CHECK(g_data_output_stream_put_int32
              (dstream, serialized_low, NULL, NULL));
        CHECK(g_data_output_stream_put_int32
              (dstream, serialized_high, NULL, NULL));

        /*
         * Save the serialized ID in the hash table, so that we don't
         * output this node again.
         */

        g_hash_table_insert(serialized_ids, node_id,
                            GINT_TO_POINTER(serialized_id));
    }

    return serialized_id;
}


gboolean
ipset_node_save(GOutputStream *stream, ipset_node_id_t node)
{
    gsize bytes_written;

    /*
     * Create a GDataOutputStream to output the binary data.
     */

    GDataOutputStream  *dstream = g_data_output_stream_new(stream);
    g_data_output_stream_set_byte_order
        (dstream, G_DATA_STREAM_BYTE_ORDER_BIG_ENDIAN);

    /*
     * First, output the magic number for an IP set, and the file
     * format version that we're going to write.
     */

    CHECK(g_output_stream_write_all
          (G_OUTPUT_STREAM(dstream),
           MAGIC_NUMBER, MAGIC_NUMBER_LENGTH,
           &bytes_written, NULL, NULL));

    CHECK(g_data_output_stream_put_uint16
          (dstream, 0x0001, NULL, NULL));

    /*
     * The rest of the output depends on whether the root is terminal
     * or nonterminal.
     */

    if (ipset_node_get_type(node) == IPSET_TERMINAL_NODE)
    {
        /*
         * If this is a terminal node, then we don't have to the
         * depth-first search.
         */

        gsize  set_size =
            MAGIC_NUMBER_LENGTH +    /* magic number */
            sizeof(guint16) +        /* version number  */
            sizeof(guint64) +        /* length of set */
            sizeof(guint32) +        /* number of nonterminals */
            sizeof(guint32);         /* terminal value */

        ipset_range_t  value = ipset_terminal_value(node);

        CHECK(g_data_output_stream_put_uint64
              (dstream, set_size, NULL, NULL));
        CHECK(g_data_output_stream_put_uint32
              (dstream, 0, NULL, NULL));
        CHECK(g_data_output_stream_put_uint32
              (dstream, value, NULL, NULL));

        g_object_unref(dstream);
        return TRUE;
    } else {
        /*
         * If this is a nonterminal node, first figure out how many
         * reachable nodes there are, to calculate the size of the
         * set.
         */

        gsize  nonterminal_count = ipset_node_reachable_count(node);

        gsize  set_size =
            MAGIC_NUMBER_LENGTH +    /* magic number */
            sizeof(guint16) +        /* version number  */
            sizeof(guint64) +        /* length of set */
            sizeof(guint32) +        /* number of nonterminals */
            (nonterminal_count *     /* for each nonterminal: */
             (sizeof(guint8) +       /*   variable number */
              sizeof(guint32) +      /*   low pointer */
              sizeof(guint32)        /*   high pointer */
             ));

        CHECK(g_data_output_stream_put_uint64
              (dstream, set_size, NULL, NULL));
        CHECK(g_data_output_stream_put_uint32
              (dstream, nonterminal_count, NULL, NULL));

        /*
         * The serialized node IDs are different than the in-memory
         * node IDs.  This means that, for our nonterminal nodes, we
         * need a mapping from internal node ID to serialized node ID.
         */

        GHashTable  *serialized_ids = g_hash_table_new(NULL, NULL);
        serialized_id_t  next_serialized_id = -1;

        serialized_id_t  last_serialized_id =
            save_visit_node(dstream, serialized_ids,
                            &next_serialized_id, node);

        g_hash_table_destroy(serialized_ids);
        g_object_unref(dstream);
        return (last_serialized_id < 0);
    }
}
