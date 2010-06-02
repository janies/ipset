/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__BDD_SET_HH
#define IP__BDD_SET_HH


#include <functional>

#include <ip/bdd/engine.hh>
#include <ip/bdd/operators.hh>


namespace ip {
namespace bdd {


/**
 * A specialization of the engine_t class, including additional
 * operators that can be applied to bool-valued BDDs.
 */

class bool_engine_t: public engine_t<bool>
{
private:
    // Shorthand typedefs

    typedef types<bool>::terminal  terminal_p;
    typedef types<bool>::nonterminal  nonterminal_p;
    typedef types<bool>::node  node_t;

    and_operator<bool>  _and;

public:
    bool_engine_t():
        _and(this)
    {
    }

    /**
     * Calculate the logical and (∧) of two BDDs.
     */

    node_t apply_and(const node_t &lhs, const node_t &rhs)
    {
        return _and(lhs, rhs);
    }
};

} // namespace bdd
} // namespace ip

#endif /* IP__BDD_SET_HH */
