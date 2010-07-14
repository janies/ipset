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
#include <utility>
#include <vector>

#include <boost/logic/tribool.hpp>
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
 * An assignment is a mapping of variable numbers to Boolean values.
 * The usual interpretation is that a Boolean function evaluates to
 * some particular value (such as TRUE) when the variables are
 * assigned according to the assignment.  A variable can be mapped to
 * an “indeterminate” value if that variable can be either TRUE or
 * FALSE in this assignment without affecting the result of the
 * function.
 */

class assignment_t
{
public:
    /**
     * The underlying variable assignments are stored in a vector of
     * tribools.  Every variable that has a TRUE or FALSE value must
     * appear in the vector.  Variables that are indeterminite must
     * only appear to prevent gaps in the vector.  Any variables
     * outside the range of the vector are assumed to be
     * indeterminite.
     */

    typedef std::vector<boost::logic::tribool>  value_list_t;

private:
    /**
     * The underlying variable assignments.
     */

    value_list_t  values;

public:
    /**
     * Create an assignment where all variables are indeterminite.
     */

    assignment_t():
        values()
    {
    }

    /**
     * Create a copy of another assignment.
     */

    assignment_t(const assignment_t &other):
        values(other.values)
    {
    }

    /**
     * Assign another assignment into this one.
     */

    assignment_t &
    operator = (const assignment_t &other)
    {
        values = other.values;
        return (*this);
    }

    /**
     * Compare two assignments for equality.
     */

    bool
    operator == (const assignment_t &other) const;

    /**
     * Compare two assignments for inequality.
     */

    bool
    operator != (const assignment_t &other) const;

    /**
     * Set the given variable, and all higher variables, to the
     * indeterminate value.
     */

    void
    cut(variable_t var)
    {
        if (var < values.size())
            values.resize(var);
    }

    /**
     * Clear the assignment, setting all variables to the
     * indeterminate value.
     */

    void
    clear()
    {
        values.clear();
    }

    /**
     * Set the value assigned to a particular variable.
     */

    boost::logic::tribool &
    operator [] (variable_t var)
    {
        // Ensure that the vector is big enough to hold this variable
        // assignment, inserting new indeterminates if needed.

        if (var >= values.size())
        {
            values.resize(var + 1, boost::logic::indeterminate);
        }

        return values[var];
    }

    /**
     * Return the value assigned to a particular variable.
     */

    boost::logic::tribool
    operator [] (variable_t var) const
    {
        if (var < values.size())
        {
            return values[var];
        } else {
            return boost::logic::indeterminate;
        }
    }


    friend
    std::ostream &
    operator << (std::ostream &stream, const assignment_t &assignment);
};


std::ostream &
operator << (std::ostream &stream, const assignment_t &assignment);


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
     * A helper method for reading a version 1 IP set stream.
     */

    bool
    load_v1(std::istream &stream, node_id_t &id);

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
     * Load a BDD from an input stream.
     */

    bool
    load(std::istream &stream, node_id_t &id);

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

    /**
     * An iterator for walking through the assignments for a given
     * node.
     *
     * The iterator walks through each path in the BDD tree, stopping
     * at each terminal node.  Each time we reach a terminal node, we
     * yield a new assignment_t object representing the assignment of
     * variables along the current path.
     *
     * We maintain a stack of nodes leading to the current terminal,
     * which allows us to backtrack up the path to find the next
     * terminal when we increment the iterator.
     */

    struct iterator
    {
    private:
        /**
         * The node cache that created this iterator.
         */

        node_cache_t  &cache;

        /**
         * Whether there are any more assignments in this iterator.
         */

        bool  finished;

        /**
         * Our node stack is simply a vector of node references.
         */

        typedef std::vector<node_t>  stack_t;

        /**
         * The sequence of nodes leading to the current terminal,
         * represented as a stack.
         */

        stack_t  stack;

    public:
        /**
         * The result returned from the iterator is a pair of the
         * assignment and the terminal value.
         */

        typedef std::pair<assignment_t, range_t>  value_t;

    private:
        /**
         * The result returned from the iterator.
         */

        value_t  result;

        /**
         * A helper method to get at the assignment part of the
         * current path.
         */

        assignment_t &
        assignment()
        {
            return result.first;
        }

        /**
         * A helper method to get at the terminal value part of the
         * current path.
         */

        range_t &
        terminal()
        {
            return result.second;
        }

        /**
         * Advance to the next path in the BDD tree.  Sets finished to
         * true if there are no more paths; otherwise fills in
         * assignment, stack, terminal, and result for the next path.
         */

        void advance();

        /**
         * Add the given node ID to the node stack, and trace down
         * from it until we find a terminal node.  Assign values to
         * the variables for each nonterminal that encounter along the
         * way.  We check low edges first, so each variable will be
         * assigned FALSE.  (The high edges will be checked eventually
         * by a call to the advance method.)
         */

        void add_node(node_id_t id);

    public:
        /**
         * Create a new iterator that points past the end of the set.
         */

        iterator(node_cache_t &cache_):
            cache(cache_),
            finished(true)
        {
        }

        /**
         * Create a new iterator for the given root node.
         */

        iterator(node_cache_t &cache_, node_id_t id):
            cache(cache_),
            finished(false)
        {
            add_node(id);
        }

        /**
         * Copy construct an iterator.
         */

        iterator(const iterator &other):
            cache(other.cache),
            finished(other.finished),
            stack(other.stack),
            result(other.result)
        {
        }

        /**
         * Copy an iterator.
         */

        iterator &
        operator = (const iterator &other)
        {
            finished = other.finished;
            stack = other.stack;
            result = other.result;

            return (*this);
        }

        /**
         * Compare two iterators for equality.
         */

        bool
        operator == (const iterator &other) const
        {
            // If either iterator is finished, then the other must be,
            // too.

            if (finished || other.finished)
                return (finished && other.finished);

            // Otherwise, comparing the assignments and terminal
            // values is enough.

            return (result == other.result);
        }

        /**
         * Compare two pointers for inequality.
         */

        bool
        operator != (const iterator &other) const
        {
            return !(this->operator == (other));
        }

        /**
         * Dereference the iterator, returning the assignment and
         * terminal value of the current path.
         */

        const value_t &
        operator * () const
        {
            return result;
        }

        /**
         * Dereference the iterator, returning the assignment and
         * terminal value of the current path.
         */

        value_t &
        operator * ()
        {
            return result;
        }

        /**
         * Dereference the iterator, returning the assignment and
         * terminal value of the current path.
         */

        const value_t *
        operator -> () const
        {
            return &result;
        }

        /**
         * Dereference the iterator, returning the assignment and
         * terminal value of the current path.
         */

        value_t *
        operator -> ()
        {
            return &result;
        }

        /**
         * Advance the iterator to the next assignment.
         */

        iterator &
        operator ++ ()
        {
            advance();
            return (*this);
        }

        /**
         * Advance the iterator to the next assignment.
         */

        iterator
        operator ++ (int dummy)
        {
            iterator  saved(*this);
            advance();
            return saved;
        }
    };


    /**
     * Return an iterator that yields all of the assignments in the
     * given BDD.  The iterator returns a pair; the first element is
     * an assignment_t providing the value that each variable takes,
     * while the second is the terminal value that is the result of
     * the Boolean function.
     */

    iterator begin(node_id_t node)
    {
        return iterator(*this, node);
    }


    /**
     * Return an iterator that points past the end of any assignments
     * in any BDD.
     */

    iterator end()
    {
        return iterator(*this);
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_NODES_HH */
