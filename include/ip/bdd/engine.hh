/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__BDD_ENGINE_HH
#define IP__BDD_ENGINE_HH


#include <functional>

#include <boost/tuple/tuple.hpp>

#include <ip/bdd/nodes.hh>
#include <ip/bdd/operators.hh>


namespace ip {
namespace bdd {


/**
 * An “engine” for handling BDD nodes with Boolean terminals.  This
 * includes all of the necessary caches for the various BDD functions
 * and operators.  Any BDD created using the same engine instance will
 * share nodes, which can be a big memory saver in some use cases.
 */

class bool_engine_t
{
private:
    /**
     * A cache of the BDD nodes.
     */

    node_cache_t  _cache;

    // Cached binary operators

    binary_operator<std::logical_and<bool> >  _and;
    binary_operator<std::logical_or<bool> >  _or;

public:
    /**
     * Create a new, empty node cache.
     */

    bool_engine_t():
        _cache(),
        _and(_cache),
        _or(_cache)
    {
    }

    /**
     * Return the index of the corresponding nonterminal node,
     * creating it if necessary.
     */

    node_id_t
    nonterminal(variable_t variable, node_id_t low, node_id_t high)
    {
        return _cache.nonterminal(variable, low, high);
    }

    /**
     * Return a reference to the nonterminal node with the given ID.
     * The result is undefined if you pass in a terminal ID.
     */

    const node_t &
    node(node_id_t id)
    {
        return _cache.node(id);
    }

    /**
     * Return the number of nodes that are reachable from the given
     * node.  This does not include duplicates if a node is reachable
     * via more than one path.
     */

    size_t
    reachable_node_count(node_id_t id) const
    {
        return _cache.reachable_node_count(id);
    }

    /**
     * Return the amount of memory used by the nodes in the given BDD.
     */

    size_t
    memory_size(node_id_t id) const
    {
        return _cache.memory_size(id);
    }

    /**
     * Persist a BDD to an output stream.  This encodes the set using
     * only those nodes that are reachable from the BDD's root node.
     */

    void
    save(std::ostream &stream, node_id_t id) const
    {
        _cache.save(stream, id);
    }

    /**
     * Evaluate a BDD given a particular variable assignment.  The
     * variable assignment should a vector-like class whose elements
     * can be cast to bools.  It should contain enough elements for
     * all of the variables in the BDD tree.
     */

    template <typename Assignment>
    range_t evaluate(node_id_t node_id,
                     const Assignment &variables) const
    {
        return _cache.evaluate(node_id, variables);
    }

    /**
     * Return the terminal node for the false value.
     */

    node_id_t false_node() const
    {
        return _cache.terminal(false);
    }

    /**
     * Return the terminal node for the true value.
     */

    node_id_t true_node() const
    {
        return _cache.terminal(true);
    }

    /**
     * Calculate the logical and (∧) of two BDDs.
     */

    node_id_t
    apply_and(node_id_t lhs, node_id_t rhs)
    {
        return _and(lhs, rhs);
    }

    /**
     * Calculate the logical or (∨) of two BDDs.
     */

    node_id_t
    apply_or(node_id_t lhs, node_id_t rhs)
    {
        return _or(lhs, rhs);
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_ENGINE_HH */
