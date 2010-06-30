/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__BDD_NODES_HH
#define IP__BDD_NODES_HH


/**
 * @file
 *
 * This file contains a very basic BDD implementation for storing IP
 * sets.
 */

#include <map>
#include <ostream>
#include <vector>

#include <boost/tuple/tuple.hpp>
#include <glog/logging.h>

#include <ip/bdd/cache.hh>


namespace ip {
namespace bdd {


// Forward declarations

class node_cache_t;


/**
 * Each variable in a BDD is referred to by number.
 */

typedef unsigned int  variable_t;


/**
 * Each BDD terminal represents an integer value.
 */

typedef int  range_t;


/**
 * An identifier for each distinct node in a BDD.  Negative values
 * represent non-terminals, non-negative values represent terminals.
 */

typedef int  node_id_t;


/**
 * A nonterminal BDD node.  This is an inner node of the BDD tree.
 * The node represents one variable in an overall variable assignment.
 * The node has two children: a “low” child and a “high” child.  The
 * low child is the subtree that applies when the node's variable is
 * false or 0; the high child is the subtree that applies when it's
 * true or 1.
 *
 * This class does not take care of ensuring that all BDD nodes are
 * reduced; that is handled by the node_cache_t class.
 */

class node_t
{
private:
    /**
     * The variable that this node represents.
     */

    variable_t  _variable;

    /**
     * The subtree node for when the variable is false.
     */

    node_id_t  _low;

    /**
     * The subtree node for when the variable is true.
     */

    node_id_t  _high;

public:
    /**
     * Create a new nonterminal node for the given variable and
     * subtrees.  Most user code should not create node_t objects by
     * hand; instead, they should use the node_cache_t class.
     *
     * Note that we do not ensure that nonterminal nodes are reduced:
     * it's perfectly valid to instantiate two node_t objects by hand
     * with the same contents.  If you do so, those two objects will
     * live at different memory locations, and won't be reduced.
     */

    node_t(const variable_t variable_,
           const node_id_t &low_,
           const node_id_t &high_):
        _variable(variable_),
        _low(low_),
        _high(high_)
    {
    }

    /**
     * Create a copy of a nonterminal node.
     */

    node_t(const node_t &other):
        _variable(other._variable),
        _low(other._low),
        _high(other._high)
    {
    }

    /**
     * Return the variable that this node represents.
     */

    variable_t variable() const
    {
        return _variable;
    }

    /**
     * Return the ID of the subtree node for when the variable is
     * false.
     */

    node_id_t low() const
    {
        return _low;
    }

    /**
     * Return the ID of the subtree node for when the variable is
     * true.
     */

    node_id_t high() const
    {
        return _high;
    }

    /**
     * Test two nonterminals for equality.  Note that this operator is
     * usually not the one that will be used; reduced nodes are
     * compared by their memory address, which ends up using
     * shared_ptr's implementation of the operator.
     */

    bool operator == (const node_t &other) const
    {
        return
            (_variable == other._variable) &&
            (_low == other._low) &&
            (_high == other._high);
    }

    /**
     * Compare two nonterminals by value.  Note that this operator is
     * usually not the one that will be used; reduced nodes are
     * compared by their memory address, which ends up using
     * shared_ptr's implementation of the operator.
     */

    bool operator < (const node_t &other) const
    {
        if (_variable < other._variable)
            return true;
        else if (_variable > other._variable)
            return false;

        if (_low < other._low)
            return true;
        else if (_low > other._low)
            return false;

        return (_high < other._high);
    }
};


std::ostream &
operator << (std::ostream &stream, const node_t &node);


/**
 * A cache for BDD nodes.  By creating and retrieving nodes through
 * the cache, we ensure that a BDD is reduced.
 */

class node_cache_t
{
private:
    /**
     * A vector of the node objects that have been created.
     */

    typedef std::vector<node_t>  node_list_t;

    node_list_t  _nodes;

    /**
     * Convert between an index in the node vector, and the ID of the
     * corresponding nonterminal.  (Nonterminals have IDs < 0)
     */

    static node_list_t::size_type
    node_id_to_index(node_id_t id)
    {
        return (-id) - 1;
    }

    /**
     * Convert between the ID of a nonterminal and its index in the
     * node vector.  (Nonterminals have IDs < 0)
     */

    static node_id_t
    index_to_node_id(node_list_t::size_type index)
    {
        return -(index + 1);
    }

    /**
     * A function object that creates a nonterminal node.  This will
     * be cached to ensure that the BDD nodes are reduced.
     */

    struct nonterminal_creator:
        cached_ternary_function_t<nonterminal_creator,
                                  variable_t, node_id_t, node_id_t,
                                  node_id_t>
    {
        node_list_t  &_nodes;

        nonterminal_creator(node_list_t &nodes_):
            _nodes(nodes_)
        {
        }

        node_id_t
        call(variable_t variable, node_id_t low, node_id_t high);
    };

    /**
     * The cached function for creating nonterminal nodes.
     */

    nonterminal_creator  _nonterminals;

    /**
     * A helper method for the save() method.  Outputs a nonterminal
     * node in a BDD tree, if we haven't done so already.  Ensures
     * that the children of the nonterminal are output before the
     * nonterminal is.
     */

    node_id_t
    save_visit_node(std::ostream &stream,
                    std::vector<node_id_t> &serialized_ids,
                    node_id_t &next_serialized_id,
                    node_id_t id) const;

public:
    /**
     * Create a new, empty node cache.
     */

    node_cache_t():
        _nodes(),
        _nonterminals(_nodes)
    {
    }

    /**
     * Return the index of a terminal node.
     */

    node_id_t
    terminal(range_t value) const
    {
        return value;
    }

    /**
     * Return the index of a nonterminal node, creating it if
     * necessary.
     */

    node_id_t
    nonterminal(variable_t variable, node_id_t low, node_id_t high)
    {
        // Don't allow any nonterminals whose low and high subtrees
        // are the same, since the nonterminal would be redundant.

        if (low == high)
        {
            DVLOG(3) << "Skipping nonterminal("
                     << variable << ','
                     << low << ','
                     << high << ')';
            return low;
        }

        return _nonterminals(variable, low, high);
    }

    /**
     * Return a reference to the nonterminal node with the given ID.
     * The result is undefined if you pass in a terminal ID.
     */

    const node_t &
    node(node_id_t id) const
    {
        return _nodes[node_id_to_index(id)];
    }

    /**
     * Return the number of nodes that are reachable from the given
     * node.  This does not include duplicates if a node is reachable
     * via more than one path.
     */

    size_t
    reachable_node_count(node_id_t id) const;

    /**
     * Return the amount of memory used by the nodes in the given BDD.
     */

    size_t
    memory_size(node_id_t id) const
    {
        return reachable_node_count(id) * sizeof(node_t);
    }

    /**
     * Persist a BDD to an output stream.  This encodes the set using
     * only those nodes that are reachable from the BDD's root node.
     */

    void
    save(std::ostream &stream, node_id_t id) const;

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
        // As long as the current node is a nonterminal, we have to
        // check the value of the current variable.

        while (node_id < 0)
        {
            // We have to look up this variable in the assignment.

            const node_t  &node = this->node(node_id);

            if (variables[node.variable()])
            {
                // This node's variable is true in the assignment vector,
                // so trace down the high subtree.

                node_id = node.high();
            } else {
                // This node's variable is false in the assignment vector,
                // so trace down the low subtree.

                node_id = node.low();
            }
        }

        // Once we find a terminal node, we've got the final result.

        return node_id;
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_NODES_HH */
