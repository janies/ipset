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


/**
 * A helper method that calls a function that expects a GError
 * location as its last parameter.  If the function raises an error,
 * that error is propagated into the “err” parameter (which should be
 * the last parameter of the enclosing function).  We then “return”
 * the desired value by assigning it to the “result” variable (which
 * you must define), and branching to a label called “error” (which
 * you must also define), which can contain any cleanup code (this
 * equivalent of a finally block).
 */

#define TRY_OR_RETURN(result_val, func, ...)    \
    G_STMT_START {                              \
        GError  *suberror = NULL;               \
        func(__VA_ARGS__, &suberror);           \
        if (suberror != NULL)                   \
        {                                       \
            g_propagate_error(err, suberror);   \
            result = (result_val);              \
            goto error;                         \
        }                                       \
    } G_STMT_END


/*-----------------------------------------------------------------------
 * Generic saving logic
 */

/**
 * On disk, we use a different node ID scheme than we do in memory.
 * Terminal node IDs are non-negative, and are equal to the terminal
 * value.  Nonterminal node IDs are negative, starting with -1.
 * Nonterminal -1 appears first on disk, then nonterminal -2, and so
 * on.
 */

typedef gint  serialized_id_t;


/* forward declaration */

typedef struct save_data save_data_t;


/**
 * A callback that outputs any necessary header.  Should return a
 * gboolean indicating whether the write was successful.
 */

typedef gboolean
(*write_header_t)(save_data_t *save_data,
                  ipset_node_cache_t *cache,
                  ipset_node_id_t root,
                  GError **err);


/**
 * A callback that outputs any necessary footer.  Should return a
 * gboolean indicating whether the write was successful.
 */

typedef gboolean
(*write_footer_t)(save_data_t *save_data,
                  ipset_node_cache_t *cache,
                  ipset_node_id_t root,
                  GError **err);


/**
 * A callback that actually outputs a terminal node to disk.  Should
 * return a gboolean indicating whether the write was successful.
 */

typedef gboolean
(*write_terminal_t)(save_data_t *save_data,
                    ipset_range_t terminal_value,
                    GError **err);


/**
 * A callback that actually outputs a nonterminal node to disk.
 * Should return a gboolean indicating whether the write was
 * successful.
 */

typedef gboolean
(*write_nonterminal_t)(save_data_t *save_data,
                       serialized_id_t serialized_id,
                       ipset_variable_t variable,
                       serialized_id_t serialized_low,
                       serialized_id_t serialized_high,
                       GError **err);


/**
 * A helper struct containing all of the persistent data items needed
 * during the execution of a save.
 */

struct save_data
{
    /**
     * The output stream to save the data to.
     */

    GDataOutputStream  *dstream;

    /**
     * The cache of serialized IDs for any nonterminals that we've
     * encountered so far.
     */

    GHashTable  *serialized_ids;

    /**
     * The serialized ID to use for the next nonterminal that we
     * encounter.
     */

    serialized_id_t  next_serialized_id;

    /**
     * The callback used to write the file header to the stream.
     */

    write_header_t  write_header;

    /**
     * The callback used to write the file footer to the stream.
     */

    write_footer_t  write_footer;

    /**
     * The callback used to write terminals to the stream.
     */

    write_terminal_t  write_terminal;

    /**
     * The callback used to write nonterminals to the stream.
     */

    write_nonterminal_t  write_nonterminal;

    /**
     * A pointer to any additional data needed by the callbacks.
     */

    gpointer  user_data;
};


/**
 * A helper function for ipset_node_save().  Outputs a nonterminal
 * node in a BDD tree, if we haven't done so already.  Ensures that
 * the children of the nonterminal are output before the nonterminal
 * is.  Return the serialized ID of this node.
 */

serialized_id_t
save_visit_node(save_data_t *save_data,
                ipset_node_id_t node_id,
                GError **err)
{
    serialized_id_t  result;

    /*
     * Check whether we've already serialized this node.
     */

    gpointer  serialized_ptr;
    gboolean  node_exists =
        g_hash_table_lookup_extended
        (save_data->serialized_ids,
         node_id,
         NULL,
         &serialized_ptr);

    if (node_exists)
    {
        result = GPOINTER_TO_INT(serialized_ptr);
    } else {
        if (ipset_node_get_type(node_id) == IPSET_TERMINAL_NODE)
        {
            /*
             * For terminals, there isn't really anything to do — we
             * just output the terminal node and use its value as the
             * serialized ID.
             */

            ipset_range_t  value = ipset_terminal_value(node_id);

            g_d_debug("Writing terminal(%d)", value);

            TRY_OR_RETURN(0,
                          save_data->write_terminal,
                          save_data,
                          value);

            result = value;
        } else {
            /*
             * For nonterminals, we drill down into the node's
             * children first, then output the nonterminal node.
             */

            ipset_node_t  *node = ipset_nonterminal_node(node_id);

            g_d_debug("Visiting node %p nonterminal(%u,%p,%p)",
                      node_id, node->variable, node->low, node->high);

            /*
             * Output the node's nonterminal children before we output
             * the node itself.
             */

            serialized_id_t  serialized_low;
            serialized_id_t  serialized_high;

            TRY_OR_RETURN(0,
                          serialized_low = save_visit_node,
                          save_data,
                          node->low);

            TRY_OR_RETURN(0,
                          serialized_high = save_visit_node,
                          save_data,
                          node->high);

            /*
             * Output the nonterminal
             */

            result = save_data->next_serialized_id--;
            g_d_debug("Writing node %p as serialized node %d"
                      " = (%u,%d,%d)",
                      node_id, result,
                      node->variable, serialized_low, serialized_high);

            TRY_OR_RETURN(0,
                          save_data->write_nonterminal,
                          save_data,
                          result, node->variable,
                          serialized_low,
                          serialized_high);
        }

        /*
         * Save the serialized ID in the hash table, so that we don't
         * output this node again.
         */

        g_hash_table_insert(save_data->serialized_ids, node_id,
                            GINT_TO_POINTER(result));
    }

    return result;

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


static gboolean
save_bdd(save_data_t *save_data,
         ipset_node_cache_t *cache,
         ipset_node_id_t root,
         GError **err)
{
    gboolean  result = FALSE;

    /*
     * First, output the file header.
     */

    g_d_debug("Writing file header");

    TRY_OR_RETURN(FALSE,
                  save_data->write_header,
                  save_data, cache, root);

    /*
     * The serialized node IDs are different than the in-memory node
     * IDs.  This means that, for our nonterminal nodes, we need a
     * mapping from internal node ID to serialized node ID.
     */

    g_d_debug("Creating file caches");

    save_data->serialized_ids = g_hash_table_new(NULL, NULL);
    save_data->next_serialized_id = -1;

    /*
     * Trace down through the BDD tree, outputting each terminal and
     * nonterminal node as they're encountered.
     */

    g_d_debug("Writing nodes");

    serialized_id_t  last_serialized_id;

    TRY_OR_RETURN(FALSE,
                  last_serialized_id = save_visit_node,
                  save_data,
                  root);

    /*
     * Finally, output the file footer and cleanup.
     */

    g_d_debug("Writing file footer");

    TRY_OR_RETURN(FALSE,
                  save_data->write_footer,
                  save_data, cache, root);

    g_d_debug("Freeing file caches");

    g_hash_table_destroy(save_data->serialized_ids);

    return TRUE;

  error:
    /*
     * If there's an error, clean up the objects that we've created
     * before returning.
     */

    if (save_data->serialized_ids != NULL)
        g_hash_table_destroy(save_data->serialized_ids);

    return result;
}


/*-----------------------------------------------------------------------
 * V1 BDD file
 */

static const char  MAGIC_NUMBER[] = "IP set";
static const gsize  MAGIC_NUMBER_LENGTH = 6;


static gboolean
write_header_v1(save_data_t *save_data,
                ipset_node_cache_t *cache,
                ipset_node_id_t root,
                GError **err)
{
    gboolean  result = FALSE;
    gsize bytes_written;

    /*
     * The data should all be big-endian.
     */

    g_data_output_stream_set_byte_order
        (save_data->dstream, G_DATA_STREAM_BYTE_ORDER_BIG_ENDIAN);

    /*
     * Output the magic number for an IP set, and the file format
     * version that we're going to write.
     */

    TRY_OR_RETURN(FALSE,
                  g_output_stream_write_all,
                  G_OUTPUT_STREAM(save_data->dstream),
                  MAGIC_NUMBER, MAGIC_NUMBER_LENGTH,
                  &bytes_written, NULL);

    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_uint16,
                  save_data->dstream, 0x0001, NULL);

    /*
     * Determine how many reachable nodes there are, to calculate the
     * size of the set.
     */

    gsize  nonterminal_count = ipset_node_reachable_count(root);

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

    /*
     * If the root is a terminal, we need to add 4 bytes to the set
     * size, for storing the terminal value.
     */

    if (ipset_node_get_type(root) == IPSET_TERMINAL_NODE)
    {
        set_size += sizeof(guint32);
    }

    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_uint64,
                  save_data->dstream, set_size, NULL);
    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_uint32,
                  save_data->dstream, nonterminal_count, NULL);

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


static gboolean
write_footer_v1(save_data_t *save_data,
                ipset_node_cache_t *cache,
                ipset_node_id_t root,
                GError **err)
{
    gboolean  result = FALSE;

    /*
     * If the root is a terminal node, then we output the terminal
     * value in place of the (nonexistent) list of nonterminal nodes.
     */

    if (ipset_node_get_type(root) == IPSET_TERMINAL_NODE)
    {
        ipset_range_t  value = ipset_terminal_value(root);

        TRY_OR_RETURN(FALSE,
                      g_data_output_stream_put_uint32,
                      save_data->dstream, value, NULL);
    }

    return TRUE;

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


static gboolean
write_terminal_v1(save_data_t *save_data,
                  ipset_range_t terminal_value,
                  GError **err)
{
    /*
     * We don't have to write anything out for a terminal in a V1
     * file, since the terminal's value will be encoded into the node
     * ID wherever it's used.
     */

    return TRUE;
}


static gboolean
write_nonterminal_v1(save_data_t *save_data,
                     serialized_id_t serialized_id,
                     ipset_variable_t variable,
                     serialized_id_t serialized_low,
                     serialized_id_t serialized_high,
                     GError **err)
{
    gboolean  result = FALSE;

    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_byte,
                  save_data->dstream, variable, NULL);
    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_int32,
                  save_data->dstream, serialized_low, NULL);
    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_int32,
                  save_data->dstream, serialized_high, NULL);

    return TRUE;

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


gboolean
ipset_node_cache_save(GOutputStream *stream,
                      ipset_node_cache_t *cache,
                      ipset_node_id_t node,
                      GError **err)
{
    g_return_val_if_fail(err == NULL || *err == NULL, FALSE);

    gboolean  result = FALSE;

    save_data_t  save_data = {
        NULL,                   /* output stream */
        NULL,                   /* serialized ID cache */
        0,                      /* next serialized ID */
        write_header_v1,        /* header writer */
        write_footer_v1,        /* footer writer */
        write_terminal_v1,      /* terminal writer */
        write_nonterminal_v1,   /* nonterminal writer */
        NULL                    /* user data */
    };

    /*
     * Create a GDataOutputStream to output the binary data.
     */

    save_data.dstream = g_data_output_stream_new(stream);

    /*
     * Then use the generic saving functions to output the file.
     */

    TRY_OR_RETURN(FALSE,
                  save_bdd,
                  &save_data, cache, node);

    g_object_unref(save_data.dstream);
    return TRUE;

  error:
    /*
     * If there's an error, clean up the objects that we've created
     * before returning.
     */

    g_object_unref(save_data.dstream);

    return result;
}


/*-----------------------------------------------------------------------
 * GraphViz dot file
 */

static const gchar  *GRAPHVIZ_HEADER =
    "strict digraph bdd {\n";

static const gchar  *GRAPHVIZ_FOOTER =
    "}\n";


typedef struct dot_data
{
    /**
     * The terminal value to leave out of the dot file.  This should
     * be the default value of the set or map.
     */

    ipset_range_t  default_value;

} dot_data_t;


static gboolean
write_header_dot(save_data_t *save_data,
                 ipset_node_cache_t *cache,
                 ipset_node_id_t root,
                 GError **err)
{
    gboolean  result = FALSE;

    /*
     * Output the opening clause of the GraphViz script.
     */

    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_string,
                  save_data->dstream,
                  GRAPHVIZ_HEADER, NULL);

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


static gboolean
write_footer_dot(save_data_t *save_data,
                 ipset_node_cache_t *cache,
                 ipset_node_id_t root,
                 GError **err)
{
    gboolean  result = FALSE;

    /*
     * Output the closing clause of the GraphViz script.
     */

    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_string,
                  save_data->dstream,
                  GRAPHVIZ_FOOTER, NULL);

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


static gboolean
write_terminal_dot(save_data_t *save_data,
                   ipset_range_t terminal_value,
                   GError **err)
{
    gboolean  result = FALSE;
    dot_data_t  *dot_data = (dot_data_t *) save_data->user_data;

    /*
     * If this terminal has the default value, skip it.
     */

    if (terminal_value == dot_data->default_value)
    {
        return TRUE;
    }

    /*
     * Output a node for the terminal value.
     */

    GString  *str = g_string_new(NULL);
    g_string_printf(str,
                    "    t%d [shape=box, label=%d];\n",
                    terminal_value,
                    terminal_value);

    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_string,
                  save_data->dstream,
                  str->str, NULL);

    g_string_free(str, TRUE);

    return TRUE;

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


static gboolean
write_nonterminal_dot(save_data_t *save_data,
                      serialized_id_t serialized_id,
                      ipset_variable_t variable,
                      serialized_id_t serialized_low,
                      serialized_id_t serialized_high,
                      GError **err)
{
    gboolean  result = FALSE;
    dot_data_t  *dot_data = (dot_data_t *) save_data->user_data;

    /*
     * Include a node for the nonterminal value.
     */

    GString  *str = g_string_new(NULL);
    g_string_printf(str,
                    "    n%d [shape=circle,label=%u];\n",
                    (-serialized_id), variable);

    /*
     * Include an edge for the low pointer.
     */

    if (serialized_low < 0)
    {
        /*
         * The low pointer is a nonterminal.
         */

        g_string_append_printf(str,
                               "    n%d -> n%d",
                               (-serialized_id),
                               (-serialized_low));
    } else {
        /*
         * The low pointer is a terminal.
         */

        ipset_range_t  low_value = (ipset_range_t) serialized_low;

        if (low_value == dot_data->default_value)
        {
            /*
             * The terminal is the default value, so instead of a real
             * terminal, connect this pointer to a dummy circle node.
             */

            g_string_append_printf(str,
                                   "    low%d [shape=circle,label=\"\"]\n"
                                   "    n%d -> low%d",
                                   (-serialized_id),
                                   (-serialized_id),
                                   (-serialized_id));
        } else {
            /*
             * The terminal isn't a default, so go ahead and output
             * it.
             */

            g_string_append_printf(str,
                                   "    n%d -> t%d",
                                   (-serialized_id),
                                   serialized_low);
        }
    }

    g_string_append_printf(str,
                           " [style=dashed,color=red]\n");

    /*
     * Include an edge for the high pointer.
     */

    if (serialized_high < 0)
    {
        /*
         * The high pointer is a nonterminal.
         */

        g_string_append_printf(str,
                               "    n%d -> n%d",
                               (-serialized_id),
                               (-serialized_high));
    } else {
        /*
         * The high pointer is a terminal.
         */

        ipset_range_t  high_value = (ipset_range_t) serialized_high;

        if (high_value == dot_data->default_value)
        {
            /*
             * The terminal is the default value, so instead of a real
             * terminal, connect this pointer to a dummy circle node.
             */

            g_string_append_printf(str,
                                   "    high%d "
                                   "[shape=circle,"
                                   "fixedsize=true,"
                                   "height=0.25,"
                                   "width=0.25,"
                                   "label=\"\"]\n"
                                   "    n%d -> high%d",
                                   (-serialized_id),
                                   (-serialized_id),
                                   (-serialized_id));
        } else {
            /*
             * The terminal isn't a default, so go ahead and output
             * it.
             */

            g_string_append_printf(str,
                                   "    n%d -> t%d",
                                   (-serialized_id),
                                   serialized_high);
        }
    }

    g_string_append_printf(str,
                           " [style=solid,color=black]\n");

    /*
     * Output the clauses to the stream.
     */

    TRY_OR_RETURN(FALSE,
                  g_data_output_stream_put_string,
                  save_data->dstream,
                  str->str, NULL);

    g_string_free(str, TRUE);

    return TRUE;

  error:
    /*
     * There's no cleanup to do on an error.
     */

    return result;
}


gboolean
ipset_node_cache_save_dot(GOutputStream *stream,
                          ipset_node_cache_t *cache,
                          ipset_node_id_t node,
                          GError **err)
{
    g_return_val_if_fail(err == NULL || *err == NULL, FALSE);

    gboolean  result = FALSE;

    dot_data_t  dot_data = {
        0                       /* default value */
    };

    save_data_t  save_data = {
        NULL,                   /* output stream */
        NULL,                   /* serialized ID cache */
        0,                      /* next serialized ID */
        write_header_dot,       /* header writer */
        write_footer_dot,       /* footer writer */
        write_terminal_dot,     /* terminal writer */
        write_nonterminal_dot,  /* nonterminal writer */
        &dot_data
    };

    /*
     * Create a GDataOutputStream to output the binary data.
     */

    save_data.dstream = g_data_output_stream_new(stream);

    /*
     * Then use the generic saving functions to output the file.
     */

    TRY_OR_RETURN(FALSE,
                  save_bdd,
                  &save_data, cache, node);

    g_object_unref(save_data.dstream);
    return TRUE;

  error:
    /*
     * If there's an error, clean up the objects that we've created
     * before returning.
     */

    g_object_unref(save_data.dstream);

    return result;
}
