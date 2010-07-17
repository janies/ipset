/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <glib.h>
#include <gio/gio.h>

#include <ipset/bdd/nodes.h>
#include <ipset/logging.h>


static const char  MAGIC_NUMBER[] = "IP set";
static const gsize  MAGIC_NUMBER_LENGTH = 6;


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


/**
 * On disk, we use a different node ID scheme than we do in memory.
 * Terminal node IDs are non-negative, and are equal to the terminal
 * value.  Nonterminal node IDs are negative, starting with -1.
 * Nonterminal -1 appears first on disk, then nonterminal -2, and so
 * on.
 */

typedef gint  serialized_id_t;


/**
 * A helper function that verifies that we've read exactly as many
 * bytes as we should, returning an error otherwise.
 */

static gboolean
verify_cap(gsize bytes_read, gsize cap, GError **err)
{
    if (bytes_read < cap)
    {
        /*
         * There's extra data at the end of the stream.
         */

        g_set_error(err,
                    IPSET_ERROR,
                    IPSET_ERROR_PARSE_ERROR,
                    "Malformed set: extra data at "
                    "end of stream.");
        return FALSE;
    } else if (bytes_read > cap) {
        /*
         * We read more data than we were supposed to.
         */

        g_set_error(err,
                    IPSET_ERROR,
                    IPSET_ERROR_PARSE_ERROR,
                    "Malformed set: read too much data.");
        return FALSE;
    }

    return TRUE;
}


/**
 * A helper function for reading a version 1 BDD stream.
 */

static ipset_node_id_t
load_v1(GDataInputStream *dstream,
        ipset_node_cache_t *cache,
        GError **err)
{
    ipset_node_id_t  result;
    GHashTable  *cache_ids = NULL;

    g_debug("Stream contains v1 IP set");

    /*
     * We've already read in the magic number and version.  Next
     * should be the length of the encoded set.
     */

    guint64  length;
    g_debug("Reading encoded length");
    TRY_OR_RETURN(0,
                  length = g_data_input_stream_read_uint64,
                  dstream, NULL);

    /*
     * The length includes the magic number, version number, and the
     * length field itself.  Remove those to get the cap on the
     * remaining stream.
     */

    gsize  bytes_read = 0;
    gsize  cap = length -
        MAGIC_NUMBER_LENGTH -
        sizeof(guint16) -
        sizeof(guint64);

    g_debug("Length cap is %" G_GSIZE_FORMAT " bytes.", cap);

    /*
     * Read in the number of nonterminals.
     */

    guint32  nonterminal_count;
    g_debug("Reading number of nonterminals");
    TRY_OR_RETURN(0,
                  nonterminal_count = g_data_input_stream_read_uint32,
                  dstream, NULL);
    bytes_read += sizeof(guint32);

    /*
     * If there are no nonterminals, then there's only a single
     * terminal left to read.
     */

    if (nonterminal_count == 0)
    {
        guint32  value;
        g_debug("Reading single terminal value");
        TRY_OR_RETURN(0,
                      value = g_data_input_stream_read_uint32,
                      dstream, NULL);
        bytes_read += sizeof(guint32);

        /*
         * We should have reached the end of the encoded set.
         */

        TRY_OR_RETURN(0,
                      verify_cap,
                      bytes_read, cap);

        /*
         * Create a terminal node for this value and return it.
         */

        return ipset_node_cache_terminal(cache, value);
    }

    /*
     * Otherwise, read in each nonterminal.  We need to keep track of
     * a mapping between each nonterminal's ID in the stream (which
     * are number consecutively from -1), and its ID in the node cache
     * (which could be anything).
     */

    cache_ids = g_hash_table_new(NULL, NULL);

    guint  i;
    for (i = 0; i < nonterminal_count; i++)
    {
        serialized_id_t  serialized_id = -(i+1);

        /*
         * Each serialized node consists of a variable index, a low
         * pointer, and a high pointer.
         */

        guint8  variable;
        TRY_OR_RETURN(0,
                      variable = g_data_input_stream_read_byte,
                      dstream, NULL);
        bytes_read += sizeof(guint8);

        gint32  low;
        TRY_OR_RETURN(0,
                      low = g_data_input_stream_read_int32,
                      dstream, NULL);
        bytes_read += sizeof(gint32);

        gint32  high;
        TRY_OR_RETURN(0,
                      high = g_data_input_stream_read_int32,
                      dstream, NULL);
        bytes_read += sizeof(gint32);

        g_d_debug("Read serialized node %d = (%d,"
                  "%" G_GINT32_FORMAT ","
                  "%" G_GINT32_FORMAT ")",
                  serialized_id, variable, low, high);

        /*
         * Turn the low pointer into a node ID.  If the pointer is >=
         * 0, it's a terminal value.  Otherwise, its a nonterminal ID,
         * indexing into the serialized nonterminal array.
         */

        ipset_node_id_t  low_id;

        if (low >= 0)
        {
            low_id = ipset_node_cache_terminal(cache, low);
        } else {
            /*
             * The file format guarantees that any node reference
             * points to a node earlier in the serialized array.  That
             * means we can assume that cache_ids has already been
             * filled in for this node.
             */

            low_id = g_hash_table_lookup
                (cache_ids, GINT_TO_POINTER(low));

            g_d_debug("  Serialized ID %" G_GINT32_FORMAT
                      " is internal ID %p", low, low_id);
        }

        /*
         * Do the same for the high pointer.
         */

        ipset_node_id_t  high_id;

        if (high >= 0)
        {
            high_id = ipset_node_cache_terminal(cache, high);
        } else {
            /*
             * The file format guarantees that any node reference
             * points to a node earlier in the serialized array.  That
             * means we can assume that cache_ids has already been
             * filled in for this node.
             */

            high_id = g_hash_table_lookup
                (cache_ids, GINT_TO_POINTER(high));

            g_d_debug("  Serialized ID %" G_GINT32_FORMAT
                      " is internal ID %p", high, high_id);
        }

        /*
         * Create a nonterminal node in the node cache.
         */

        result = ipset_node_cache_nonterminal
            (cache, variable, low_id, high_id);

        g_d_debug("Internal node %p = nonterminal(%d,%p,%p)",
                  result, (int) variable, low_id, high_id);

        /*
         * Remember the internal node ID for this new node, in case
         * any later serialized nodes point to it.
         */

        g_hash_table_insert(cache_ids,
                            GINT_TO_POINTER(serialized_id),
                            result);
    }

    /*
     * We should have reached the end of the encoded set.
     */

    TRY_OR_RETURN(0,
                  verify_cap,
                  bytes_read, cap);

    /*
     * The last node is the nonterminal for the entire set.
     */

    return result;

  error:
    /*
     * If there's an error, clean up the objects that we've created
     * before returning.
     */

    if (cache_ids != NULL)
        g_hash_table_destroy(cache_ids);

    return result;
}


ipset_node_id_t
ipset_node_cache_load(GInputStream *stream,
                      ipset_node_cache_t *cache,
                      GError **err)
{
    g_return_val_if_fail(err == NULL || *err == NULL, FALSE);

    ipset_node_id_t  result;
    gsize bytes_read;

    /*
     * Create a GDataInputStream to read in the binary data.
     */

    GDataInputStream  *dstream = g_data_input_stream_new(stream);
    g_data_input_stream_set_byte_order
        (dstream, G_DATA_STREAM_BYTE_ORDER_BIG_ENDIAN);

    /*
     * First, read in the magic number from the stream to ensure that
     * this is an IP set.
     */

    guint8  magic[MAGIC_NUMBER_LENGTH];

    g_debug("Reading IP set magic number");
    TRY_OR_RETURN(0,
                  g_input_stream_read_all,
                  G_INPUT_STREAM(dstream),
                  magic, MAGIC_NUMBER_LENGTH,
                  &bytes_read, NULL);

    if (bytes_read != MAGIC_NUMBER_LENGTH)
    {
        /*
         * We reached EOF before reading the entire magic number.
         */

        g_set_error(err,
                    IPSET_ERROR,
                    IPSET_ERROR_PARSE_ERROR,
                    "Unexpected end of file");
        return 0;
    }

    if (memcmp(magic, MAGIC_NUMBER, MAGIC_NUMBER_LENGTH) != 0)
    {
        /*
         * The magic number doesn't match, so this isn't a BDD.
         */

        g_set_error(err,
                    IPSET_ERROR,
                    IPSET_ERROR_PARSE_ERROR,
                    "Magic number doesn't match; "
                    "this isn't an IP set.");
        return 0;
    }

    /*
     * Read in the version number and dispatch to the right reading
     * function.
     */

    guint16  version;
    g_debug("Reading IP set version");
    TRY_OR_RETURN(0,
                  version = g_data_input_stream_read_uint16,
                  dstream, NULL);


    switch (version)
    {
      case 0x0001:
        TRY_OR_RETURN(0,
                      result = load_v1,
                      dstream, cache);
        return result;

      default:
        /*
         * We don't know how to read this version number.
         */

        g_set_error(err,
                    IPSET_ERROR,
                    IPSET_ERROR_PARSE_ERROR,
                    "Unknown version number %" G_GUINT16_FORMAT,
                    version);
        return 0;
    }

  error:
    /*
     * If there's an error, clean up the objects that we've created
     * before returning.
     */

    if (dstream != NULL)
        g_object_unref(dstream);

    return result;
}
