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
        public std::unary_function<Value, terminal_p>
    {
        terminal_p
        operator () (const Value &value) const
        {
            return terminal_p(new terminal_t<Value>(value));
        }
    };

    /**
     * A tuple type for the input to the nonterminal_t constructor.
     */

    typedef boost::tuple<variable_t, node_t, node_t>
    nonterminal_key_t;

    /**
     * A function object that creates a nonterminal instance.  This
     * will be cached to ensure that the BDD nodes are reduced.
     */

    struct nonterminal_creator:
        public std::unary_function<nonterminal_key_t, nonterminal_p>
    {
        nonterminal_p
        operator () (const nonterminal_key_t &key) const
        {
            return nonterminal_p(new nonterminal_t<Value>
                                 (boost::get<0>(key),
                                  boost::get<1>(key),
                                  boost::get<2>(key)));
        }
    };

    /**
     * The cached function for creating terminal nodes.
     */

    cached_function_t<terminal_creator>  _terminals;

    /**
     * The cached function for creating nonterminal nodes.
     */

    cached_function_t<nonterminal_creator>  _nonterminals;

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

    node_t nonterminal(variable_t variable, node_t low, node_t high)
    {
        return _nonterminals(boost::make_tuple(variable, low, high));
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_ENGINE_HH */
