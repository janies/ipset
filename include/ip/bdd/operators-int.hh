/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__BDD_OPERATORS_INT_HH
#define IP__BDD_OPERATORS_INT_HH


#include <glog/logging.h>

#include <ip/bdd/operators.hh>


namespace ip {
namespace bdd {


template <typename Operator>
node_id_t
binary_operator<Operator>::recurse
(node_id_t lhs, node_id_t rhs)
{
    return this->cached_call(boost::make_tuple(lhs, rhs));
}


template <typename Operator>
node_id_t
binary_operator<Operator>::recurse_left
(const node_t &lhs, node_id_t rhs)
{
    node_id_t  result_low = recurse(lhs.low(), rhs);
    node_id_t  result_high = recurse(lhs.high(), rhs);

    return node_cache.nonterminal
        (lhs.variable(), result_low, result_high);
}


template <typename Operator>
node_id_t
binary_operator<Operator>::recurse_both
(const node_t &lhs, const node_t &rhs)
{
    node_id_t  result_low = recurse(lhs.low(), rhs.low());
    node_id_t  result_high = recurse(lhs.high(), rhs.high());

    return node_cache.nonterminal
        (lhs.variable(), result_low, result_high);
}


template <typename Operator>
node_id_t
binary_operator<Operator>::call
(node_id_t lhs, node_id_t rhs)
{
    DVLOG(2) << "Applying " << operator_name<Operator>::op_name()
             << " to " << lhs << " and " << rhs;

    if (lhs >= 0)
    {
        if (rhs >= 0)
        {
            // When both nodes are terminal, we apply the operator to
            // the terminals' values, and construct a new terminal
            // from the result.  Note that we do not verify that the
            // operator returns a positive value.

            return op(lhs, rhs);
        } else {
            // When one node is terminal, and the other is
            // nonterminal, we recurse down the subtrees of the
            // nonterminal, combining the results with the terminal.

            const node_t  &rhs_node = node_cache.node(rhs);
            return recurse_left(rhs_node, lhs);
        }
    } else {
        if (rhs >= 0)
        {
            // When one node is terminal, and the other is
            // nonterminal, we recurse down the subtrees of the
            // nonterminal, combining the results with the terminal.

            const node_t  &lhs_node = node_cache.node(lhs);
            return recurse_left(lhs_node, rhs);
        } else {
            // When both nodes are nonterminal, the way we recurse
            // depends on the variables of the nonterminals.  We
            // always recurse down the nonterminal with the smaller
            // variable index.  This ensures that our BDDs remain
            // ordered.

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


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_OPERATORS_INT_HH */
