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

#include <ip/bdd/engine.hh>
#include <ip/bdd/nodes.hh>


namespace ip {
namespace bdd {


/**
 * A function that applies a binary operator to two BDDs.  The
 * operator itself is used on the terminal nodes; we use the standard
 * recursive APPLY function to apply the operator to nonterminal
 * nodes.  We need a reference to the engine that caches the nodes, so
 * that the BDDs that we produce are reduced.  We also ensure that the
 * BDDs are ordered, assuming that the input BDDs are also ordered.
 * Finally, this is a cached function, so we won't needlessly repeat
 * work that we've already calculated.
 */

template <typename Value, typename Operator>
struct binary_operator:
        public cached_binary_function_t
            <binary_operator<Value, Operator>,
             typename types<Value>::node,
             typename types<Value>::node,
             typename types<Value>::node>
{
private:
    // Shorthand typedefs

    typedef typename types<Value>::terminal  terminal_p;
    typedef typename types<Value>::nonterminal  nonterminal_p;
    typedef typename types<Value>::node  node_t;

public:
    typedef node_t  first_argument_type;
    typedef node_t  second_argument_type;
    typedef node_t  result_type;

private:
    /**
     * The engine that we should use to construct new nodes.  By
     * constructing them through the engine, we ensure that the BDDs
     * are reduced.
     */

    engine_t<Value>  *engine;

    /**
     * The operator function that will be applied to the terminal
     * values.
     */

    Operator  op;

    /**
     * Apply the operator recursively to the subtrees of the current
     * LHS, combining the recursive results with the current RHS.
     */

    node_t
    recurse_left(const nonterminal_p &lhs, const node_t &rhs)
    {
        node_t  result_low = (*this)(lhs->low(), rhs);
        node_t  result_high = (*this)(lhs->high(), rhs);

        return engine->nonterminal
            (lhs->variable(), result_low, result_high);
    }

    /**
     * Apply the operator recursively to the subtrees of the current
     * LHS and RHS simultaneously, combining the results.
     */

    node_t
    recurse_both(const nonterminal_p &lhs,
                 const nonterminal_p &rhs)
    {
        node_t  result_low = (*this)(lhs->low(), rhs->low());
        node_t  result_high = (*this)(lhs->high(), rhs->high());

        return engine->nonterminal
            (lhs->variable(), result_low, result_high);
    }

public:
    explicit binary_operator(engine_t<Value> *engine_):
        engine(engine_)
    {
    }

    /**
     * Apply the operator to two terminal nodes.  In this case, we
     * apply the operator to the terminals' values, and construct a
     * new terminal from the result.
     */

    node_t
    operator () (const terminal_p &lhs,
                 const terminal_p &rhs)
    {
        return engine->terminal(op(lhs->value(), rhs->value()));
    }

    /**
     * Apply the operator to a terminal and a nonterminal.  In this
     * case, we recurse down the subtrees of the nonterminal,
     * combining the results with the terminal.
     */

    node_t
    operator () (const nonterminal_p &lhs,
                 const terminal_p &rhs)
    {
        return recurse_left(lhs, rhs);
    }

    /**
     * Apply the operator to a terminal and a nonterminal.  In this
     * case, we recurse down the subtrees of the nonterminal,
     * combining the results with the terminal.
     */

    node_t
    operator () (const terminal_p &lhs,
                 const nonterminal_p &rhs)
    {
        return recurse_left(rhs, lhs);
    }

    /**
     * Apply the operator to two nonterminals.  In this case, the way
     * we recurse depends on the variables of the nonterminals.  We
     * always recurse down the nonterminal with the smaller variable
     * index.  This ensures that our BDDs remain ordered.
     */

    node_t
    operator () (const nonterminal_p &lhs,
                 const nonterminal_p &rhs)
    {
        variable_t  v_lhs = lhs->variable();
        variable_t  v_rhs = rhs->variable();

        if (v_lhs == v_rhs)
        {
            return recurse_both(lhs, rhs);
        } else if (v_lhs < v_rhs) {
            return recurse_left(lhs, rhs);
        } else {
            return recurse_left(rhs, lhs);
        }
    }

    /**
     * Apply the operator to two nodes.  Call the underlying function,
     * if necessary; return the result from our cache if we've already
     * calculated it.
     */

    node_t operator () (const node_t &lhs, const node_t &rhs)
    {
        return cached_call(boost::make_tuple(lhs, rhs));
    }

    /**
     * Apply the operator to two nodes when we haven't already done
     * so.  Because of the operator () definitions, this class can be
     * used as Boost Variant binary visitor.
     */

    node_t
    call(const node_t &lhs, const node_t &rhs)
    {
        return boost::apply_visitor(*this, lhs.variant(), rhs.variant());
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_OPERATORS_HH */
