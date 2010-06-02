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

#include <ip/bdd/cache.hh>
#include <ip/bdd/nodes.hh>


namespace ip {
namespace bdd {


/**
 * An “engine” for handling BDDs that map to a particular value type.
 * This includes all of the necessary caches for the various BDD
 * functions and operators.  Any BDD created using the same engine
 * instance will share nodes, which can be a big memory saver in some
 * use cases.
 */

template <typename Value>
class engine_t
{
private:
    // Shorthand typedefs

    typedef typename types<Value>::terminal  terminal_p;
    typedef typename types<Value>::nonterminal  nonterminal_p;
    typedef typename types<Value>::node  node_t;

    /**
     * A function object that creates a terminal instance.  This will
     * be cached to ensure that the BDD nodes are reduced.
     */

    struct terminal_creator:
        cached_unary_function_t<terminal_creator,
                                Value, terminal_p>
    {
        terminal_p
        call(const Value &value) const
        {
            return terminal_p(new terminal_t<Value>(value));
        }
    };

    /**
     * A function object that creates a nonterminal instance.  This
     * will be cached to ensure that the BDD nodes are reduced.
     */

    struct nonterminal_creator:
        cached_ternary_function_t<nonterminal_creator,
                                  variable_t, node_t, node_t,
                                  nonterminal_p>
    {
        nonterminal_p
        call(const variable_t variable,
             const node_t &low,
             const node_t &high) const
        {
            return nonterminal_p(new nonterminal_t<Value>
                                 (variable, low, high));
        }
    };

    /**
     * The cached function for creating terminal nodes.
     */

    terminal_creator  _terminals;

    /**
     * The cached function for creating nonterminal nodes.
     */

    nonterminal_creator  _nonterminals;

public:
    /**
     * Create a new BDD engine.
     */

    engine_t()
    {
    }

    /**
     * Create or retrieve the terminal for the specified value.  We
     * ensure that the nodes are reduced; if you call this function
     * with the same values more than once, you'll get the same
     * physical node each time.
     */

    node_t terminal(Value value)
    {
        return _terminals(value);
    }

    /**
     * Create or retrieve the nonterminal for the specified value.  We
     * ensure that the nodes are reduced; if you call this function
     * with the same values more than once, you'll get the same
     * physical node each time.
     */

    node_t nonterminal(variable_t variable,
                       const node_t &low,
                       const node_t &high)
    {
        // Don't allow any nonterminals whose low and high subtrees
        // are the same, since the nonterminal would be redundant.

        if (low == high)
            return low;

        return _nonterminals(variable, low, high);
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_ENGINE_HH */
