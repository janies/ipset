/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IPSET_BDD_NODES_H
#define IPSET_BDD_NODES_H


#include <stdio.h>

#include <glib.h>
#include <gio/gio.h>


/*-----------------------------------------------------------------------
 * Error reporting
 */

#define IPSET_ERROR ipset_error_quark()

GQuark
ipset_error_quark();


typedef enum
{
    IPSET_ERROR_PARSE_ERROR
} IpsetError;


/*-----------------------------------------------------------------------
 * Preliminaries
 */

/**
 * Each variable in a BDD is referred to by number.
 */

typedef guint  ipset_variable_t;


/**
 * Each BDD terminal represents an integer value.  The integer must be
 * non-negative, but must be within the range of the <i>signed</i>
 * integer type.
 */

typedef gint  ipset_range_t;


/**
 * An identifier for each distinct node in a BDD.
 *
 * Internal implementation note.  Since pointers are aligned to at
 * least two bytes, the ID of a terminal node has its LSB set to 1,
 * and has the terminal value stored in the remaining bits.  The ID of
 * a nonterminal node is simply a pointer to the node struct.
 */

typedef gpointer  ipset_node_id_t;


/**
 * Nodes can either be terminal or nonterminal.
 */

typedef enum ipset_node_type
{
    IPSET_TERMINAL_NODE,
    IPSET_NONTERMINAL_NODE
} ipset_node_type_t;


/**
 * Return the type of node represented by a particular node ID.
 */

ipset_node_type_t
ipset_node_get_type(ipset_node_id_t node);


/**
 * Return the number of nodes that are reachable from the given node.
 * This does not include duplicates if a node is reachable via more
 * than one path.
 */

gsize
ipset_node_reachable_count(ipset_node_id_t node);


/**
 * Return the amount of memory used by the nodes in the given BDD.
 */

gsize
ipset_node_memory_size(ipset_node_id_t node);


/**
 * Save a BDD to an output stream.  This encodes the set using only
 * those nodes that are reachable from the BDD's root node.
 */

gboolean
ipset_node_save(GOutputStream *stream, ipset_node_id_t node,
                GError **error);


/*-----------------------------------------------------------------------
 * Terminal nodes
 */

/**
 * Return the value of a terminal node.  The result is undefined if
 * the node ID represents a nonterminal.
 */

ipset_range_t
ipset_terminal_value(ipset_node_id_t node_id);


/*-----------------------------------------------------------------------
 * Nonterminal nodes
 */

/**
 * A nonterminal BDD node.  This is an inner node of the BDD tree.
 * The node represents one variable in an overall variable assignment.
 * The node has two children: a “low” child and a “high” child.  The
 * low child is the subtree that applies when the node's variable is
 * false or 0; the high child is the subtree that applies when it's
 * true or 1.
 *
 * This type does not take care of ensuring that all BDD nodes are
 * reduced; that is handled by the node_cache_t class.
 */

typedef struct ipset_node
{
    /**
     * The variable that this node represents.
     */

    ipset_variable_t  variable;

    /**
     * The subtree node for when the variable is false.
     */

    ipset_node_id_t  low;

    /**
     * The subtree node for when the variable is true.
     */

    ipset_node_id_t  high;

} ipset_node_t;

/**
 * Return the node struct of a nonterminal node.  The result is
 * undefined if the node ID represents a terminal.
 */

ipset_node_t *
ipset_nonterminal_node(ipset_node_id_t node_id);

/**
 * Print out a node object.
 */

void
ipset_node_fprint(FILE *stream, ipset_node_t *node);

/**
 * Return a hash value for a node.
 */

guint
ipset_node_hash(ipset_node_t *node);

/**
 * Test two nodes for equality.
 */

gboolean
ipset_node_equal(const ipset_node_t *node1,
                 const ipset_node_t *node2);


/*-----------------------------------------------------------------------
 * Node caches
 */

/**
 * A cache for BDD nodes.  By creating and retrieving nodes through
 * the cache, we ensure that a BDD is reduced.
 */

typedef struct ipset_node_cache
{
    /**
     * A cache of the nonterminal nodes, keyed by their contents.
     */

    GHashTable  *node_cache;

    /**
     * A cache of the results of the AND operation.
     */

    GHashTable  *and_cache;

    /**
     * A cache of the results of the OR operation.
     */

    GHashTable  *or_cache;

} ipset_node_cache_t;

/**
 * Convert between an index in the node vector, and the ID of the
 * corresponding nonterminal.  (Nonterminals have IDs < 0)
 */

gsize
ipset_node_id_to_index(ipset_node_id_t id);

/**
 * Convert between the ID of a nonterminal and its index in the node
 * vector.  (Nonterminals have IDs < 0)
 */

ipset_node_id_t
ipset_index_to_node_id(gsize index);

/**
 * Create a new node cache.
 */

ipset_node_cache_t *
ipset_node_cache_new();

/**
 * Free a node cache.
 */

void
ipset_node_cache_free(ipset_node_cache_t *cache);

/**
 * Create a new terminal node with the given value, returning its ID.
 * This function ensures that there is only one node with the given
 * value in this cache.
 */

ipset_node_id_t
ipset_node_cache_terminal(ipset_node_cache_t *cache,
                          ipset_range_t value);

/**
 * Create a new nonterminal node with the given contents, returning
 * its ID.  This function ensures that there is only one node with the
 * given contents in this cache.
 */

ipset_node_id_t
ipset_node_cache_nonterminal(ipset_node_cache_t *cache,
                             ipset_variable_t variable,
                             ipset_node_id_t low,
                             ipset_node_id_t high);


/**
 * Load a BDD from an input stream.  The error field is filled in with
 * a GError object is the BDD can't be read for any reason.
 */

ipset_node_id_t
ipset_node_cache_load(GInputStream *stream,
                      ipset_node_cache_t *cache,
                      GError **error);


/*-----------------------------------------------------------------------
 * BDD operators
 */

/**
 * The key for a cache that memoizes the results of a binary BDD
 * operator.
 */

typedef struct ipset_binary_key
{
    ipset_node_id_t  lhs;
    ipset_node_id_t  rhs;
} ipset_binary_key_t;

/**
 * Return a hash value for a binary operator key.
 */

guint
ipset_binary_key_hash(ipset_binary_key_t *key);

/**
 * Test two binary operator keys for equality.
 */

gboolean
ipset_binary_key_equal(const ipset_binary_key_t *key1,
                       const ipset_binary_key_t *key2);

/**
 * Fill in the key for a commutative binary BDD operator.  This
 * ensures that reversed operands yield the same key.
 */

void
ipset_binary_key_commutative(ipset_binary_key_t *key,
                             ipset_node_id_t lhs,
                             ipset_node_id_t rhs);

/**
 * Calculate the logical AND (∧) of two BDDs.
 */

ipset_node_id_t
ipset_node_cache_and(ipset_node_cache_t *cache,
                     ipset_node_id_t lhs,
                     ipset_node_id_t rhs);

/**
 * Calculate the logical OR (∨) of two BDDs.
 */

ipset_node_id_t
ipset_node_cache_or(ipset_node_cache_t *cache,
                    ipset_node_id_t lhs,
                    ipset_node_id_t rhs);


/*-----------------------------------------------------------------------
 * Evaluating BDDs
 */

/**
 * A function that provides the value for each variable in a BDD.
 */

typedef gboolean
(*ipset_assignment_func_t)(gconstpointer user_data,
                           ipset_variable_t variable);

/**
 * An assignment function that gets the variable values from an array
 * of gbooleans.
 */

gboolean
ipset_bool_array_assignment(gconstpointer user_data,
                            ipset_variable_t variable);

/**
 * An assignment function that gets the variable values from an array
 * of bits.
 */

gboolean
ipset_bit_array_assignment(gconstpointer user_data,
                           ipset_variable_t variable);

/**
 * Evaluate a BDD given a particular assignment of variables.
 */

ipset_range_t
ipset_node_evaluate(ipset_node_id_t node,
                    ipset_assignment_func_t assignment,
                    gconstpointer user_data);


#endif  /* IPSET_BDD_NODES_H */
