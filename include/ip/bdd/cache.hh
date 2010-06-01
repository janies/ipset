/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__BDD_CACHE_HH
#define IP__BDD_CACHE_HH


#include <functional>
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>


namespace ip {
namespace bdd {


/**
 * A cache for memoizing the results of a unary_function.  The cache
 * itself is a unary_function.  When you call the cached function, it
 * will only call the underlying function if it hasn't been called
 * before for those parameters.  The underlying function is provided
 * at compile time via the template parameter.
 */

template <typename Function>
class cached_function_t:
        public std::unary_function<typename Function::argument_type,
                                   typename Function::result_type>
{
public:
    typedef typename Function::argument_type  argument_type;
    typedef typename Function::result_type  result_type;

private:
    /**
     * The type of the cache of function results.
     */

    typedef std::map<argument_type, result_type>  map_t;

    /**
     * The cache of function results.
     */

    map_t  _results;

    /**
     * The underlying function object.
     */

    Function  _function;

public:
    /**
     * Create a new cached version of the given function.
     */

    explicit cached_function_t(Function function_):
        _results(),
        _function(function_)
    {
    }

    /**
     * Create a new cached version of the given function.  This
     * constructor instantiates the underlying function object using
     * its no-argument constructor.
     */

    cached_function_t():
        _results(),
        _function()
    {
    }

    /**
     * Call the underlying function if we haven't already called it
     * with the given input value.  If we have, return the cached
     * result.
     */

    result_type
    operator () (const argument_type &input)
    {
        typename map_t::iterator  lb = _results.lower_bound(input);

        if ((lb != _results.end()) && (input == lb->first))
        {
            // There was already a cached result for this input, so
            // just return it.

            return lb->second;
        } else {
            // Call the underlying function.

            result_type  result = _function(input);

            // Cache the result.

            _results.insert(lb, typename map_t::value_type(input, result));

            // Return the result.

            return result;
        }
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_CACHE_HH */
