/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__BDD_OPERATORS_HH
#define IP__BDD_OPERATORS_HH


#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

#include <ip/bdd/cache.hh>
#include <ip/bdd/nodes.hh>


namespace ip {
namespace bdd {


/**
 * A function that applies a binary operator to two BDDs.  The
 * operator itself is used on the terminal nodes; we use the standard
 * recursive APPLY function to apply the operator to nonterminal
 * nodes.  We need a reference to a node cache, so that the BDDs that
 * we produce are reduced.  We also ensure that the BDDs are ordered,
 * assuming that the input BDDs are also ordered.  Finally, this is a
 * cached function, so we won't needlessly repeat work that we've
 * already calculated.
 */

template <typename Operator>
struct binary_operator:
        public cached_binary_function_t
            <binary_operator<Operator>,
             node_id_t, node_id_t, node_id_t>
{
public:
    typedef node_id_t  first_argument_type;
    typedef node_id_t  second_argument_type;
    typedef node_id_t  result_type;

private:
    /**
     * The node cache that we should use to construct new nodes.  By
     * constructing them through the cache, we ensure that the BDDs
     * are reduced.
     */

    node_cache_t  &node_cache;

    /**
     * The operator function that will be applied to the terminal
     * values.
     */

    Operator  op;

    /**
     * Apply the operator to two nodes.  Call the underlying function,
     * if necessary; return the result from our cache if we've already
     * calculated it.
     */

    node_id_t
    recurse(node_id_t lhs, node_id_t rhs)
    {
        return this->cached_call(boost::make_tuple(lhs, rhs));
    }

    /**
     * Apply the operator recursively to the subtrees of the current
     * LHS, combining the recursive results with the current RHS.
     */

    node_id_t
    recurse_left(const node_t &lhs, node_id_t rhs)
    {
        node_id_t  result_low = recurse(lhs.low(), rhs);
        node_id_t  result_high = recurse(lhs.high(), rhs);

        return node_cache.nonterminal
            (lhs.variable(), result_low, result_high);
    }

    /**
     * Apply the operator recursively to the subtrees of the current
     * LHS and RHS simultaneously, combining the results.
     */

    node_id_t
    recurse_both(const node_t &lhs, const node_t &rhs)
    {
        node_id_t  result_low = recurse(lhs.low(), rhs.low());
        node_id_t  result_high = recurse(lhs.high(), rhs.high());

        return node_cache.nonterminal
            (lhs.variable(), result_low, result_high);
    }

public:
    explicit binary_operator(node_cache_t &node_cache_):
        node_cache(node_cache_)
    {
    }

    /**
     * Apply the operator to two nodes.  Our behavior depends on
     * whether the two nodes are terminal or nonterminal.
     */

    node_id_t
    call(node_id_t lhs, node_id_t rhs)
    {
        if (lhs >= 0)
        {
            if (rhs >= 0)
            {
                // When both nodes are terminal, we apply the operator
                // to the terminals' values, and construct a new
                // terminal from the result.  Note that we do not
                // verify that the operator returns a positive value.

                return op(lhs, rhs);
            } else {
                // When one node is terminal, and the other is
                // nonterminal, we recurse down the subtrees of the
                // nonterminal, combining the results with the
                // terminal.

                const node_t  &rhs_node = node_cache.node(rhs);
                return recurse_left(rhs_node, lhs);
            }
        } else {
            if (rhs >= 0)
            {
                // When one node is terminal, and the other is
                // nonterminal, we recurse down the subtrees of the
                // nonterminal, combining the results with the
                // terminal.

                const node_t  &lhs_node = node_cache.node(lhs);
                return recurse_left(lhs_node, rhs);
            } else {
                // When both nodes are nonterminal, the way we recurse
                // depends on the variables of the nonterminals.  We
                // always recurse down the nonterminal with the
                // smaller variable index.  This ensures that our BDDs
                // remain ordered.

                const node_t  &lhs_node = node_cache.node(lhs);
                const node_t  &rhs_node = node_cache.node(rhs);

                variable_t  v_lhs = lhs_node.variable();
                variable_t  v_rhs = rhs_node.variable();

                if (v_lhs == v_rhs)
                {
                    return recurse_both(lhs_node, rhs_node);
                } else if (v_lhs < v_rhs) {
                    return recurse_left(lhs_node, rhs);
                } else {
                    return recurse_left(rhs_node, lhs);
                }
            }
        }
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_OPERATORS_HH */
