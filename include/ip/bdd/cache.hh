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
 * A cache for memoizing the results of a function object.  The cache
 * itself is also a function object.  When you call the cached
 * function, it will only call the underlying function if it hasn't
 * been called before for those parameters.
 *
 * The class handles the cache itself.  The CacheKey and Result
 * template parameters define the function's input and output
 * parameters.  They must both be single types, so for multi-parameter
 * functions, CacheKey should probably be a Boost Tuple.
 *
 * The class is defined using the Curiously Recurring Template
 * Pattern.  The Derived template parameter should be a class that
 * derived from cached_function_t.  It should define a call() method
 * that actually performs the underlying function call.  The method
 * should take in a CacheKey parameter and return a Result value.
 *
 * The cached_unary_function_t, cached_binary_function_t, and
 * cached_ternary_function_t subclasses provide helper methods for
 * calling the cached function without having to box and unbox the
 * tuples that are used as cache keys.
 */

template <typename Derived,
          typename CacheKey,
          typename Result>
class cached_function_t
{
public:
    typedef CacheKey  cache_key_t;
    typedef Result  result_type;

private:
    /**
     * The type of the cache of function results.
     */

    typedef std::map<cache_key_t, result_type>  map_t;

    /**
     * The cache of function results.
     */

    map_t  _results;

protected:
    /**
     * Call the underlying function if we haven't already called it
     * with the given input value.  If we have, return the cached
     * result.
     */

    result_type
    cached_call(const cache_key_t &input)
    {
        typename map_t::iterator  lb = _results.lower_bound(input);

        if ((lb != _results.end()) && (input == lb->first))
        {
            // There was already a cached result for this input, so
            // just return it.

            return lb->second;
        } else {
            // Call the underlying function.

            result_type  result =
                static_cast<Derived *>(this)->call(input);

            // Cache the result.

            _results.insert(lb, typename map_t::value_type(input, result));

            // Return the result.

            return result;
        }
    }

public:
    /**
     * Create a new empty function cache.
     */

    cached_function_t():
        _results()
    {
    }
};


/**
 * A helper class for defining a cached unary function.  The Argument
 * and Result template parameters should be the types of the
 * function's input and output parameters.
 *
 * The Function class should derive from cached_unary_function_t.  It
 * should define a call() method that performs the actual underlying
 * call.  It should take in an Argument parameter and return a Result
 * value.
 *
 * This class defines an implementation of the () operator, which
 * should usually be inherited by Function.  This allows you to use
 * Function instances as function objects, with the calls to the
 * function object being properly cached.
 */

template <typename Function,
          typename Argument,
          typename Result>
class cached_unary_function_t:
        public cached_function_t
            <cached_unary_function_t<Function, Argument, Result>,
             Argument, Result>
{
public:
    // Some useful typedefs

    typedef Function  function_t;
    typedef Argument  argument_type;
    typedef Result  result_type;

    /**
     * The cache key for the function's cache.  Since a unary function
     * only has one argument, we don't have to box it up in a tuple.
     */

    typedef argument_type  cache_key_t;

    /**
     * Unbox the cache key and call the underlying function.  Since
     * there's only one argument type, there isn't any actual unboxing
     * to do.
     */

    result_type
    call(const cache_key_t &input)
    {
        return static_cast<Function *>(this)->call(input);
    }

    /**
     * Call the underlying function, if necessary; return the result
     * from our cache if we've already calculated it.
     */

    result_type
    operator () (const argument_type &arg)
    {
        // Box up the arguments before checking the cache.  Since
        // there's only one argument type, there isn't any actual
        // boxing to do.

        return cached_call(arg);
    }
};


/**
 * A helper class for defining a cached binary function.  The
 * Argument1, Argument2, and Result template parameters should be the
 * types of the function's input and output parameters.
 *
 * The Function class should derive from cached_binary_function_t.  It
 * should define a call() method that performs the actual underlying
 * call.  It should take in Argument1 and Argument2 parameters and
 * return a Result value.
 *
 * This class defines an implementation of the () operator, which
 * should usually be inherited by Function.  This allows you to use
 * Function instances as function objects, with the calls to the
 * function object being properly cached.
 */

template <typename Function,
          typename Argument1,
          typename Argument2,
          typename Result>
class cached_binary_function_t:
        public cached_function_t
            <cached_binary_function_t<Function, Argument1,
                                      Argument2, Result>,
             typename boost::tuple<Argument1, Argument2>,
             Result>
{
public:
    // Some useful typedefs

    typedef Function  function_t;
    typedef Argument1  first_argument_type;
    typedef Argument2  second_argument_type;
    typedef Result  result_type;

    /**
     * The cache key for the function's cache.  This is the function's
     * argument types, boxed up into a tuple.
     */

    typedef boost::tuple<Argument1, Argument2>  cache_key_t;

    /**
     * Unbox the cache key and call the underlying function.
     */

    result_type
    call(const cache_key_t &input)
    {
        return static_cast<Function *>(this)->
            call(boost::get<0>(input),
                 boost::get<1>(input));
    }

    /**
     * Call the underlying function, if necessary; return the result
     * from our cache if we've already calculated it.
     */

    result_type
    operator () (const first_argument_type &arg1,
                 const second_argument_type &arg2)
    {
        // Box up the arguments before checking the cache.

        return cached_call(boost::make_tuple(arg1, arg2));
    }
};


/**
 * A helper class for defining a cached ternary function.  The
 * Argument1, Argument2, Argument3, and Result template parameters
 * should be the types of the function's input and output parameters.
 *
 * The Function class should derive from cached_ternary_function_t.
 * It should define a call() method that performs the actual
 * underlying call.  It should take in Argument1, Argument2, and
 * Argument3 parameters, and return a Result value.
 *
 * This class defines an implementation of the () operator, which
 * should usually be inherited by Function.  This allows you to use
 * Function instances as function objects, with the calls to the
 * function object being properly cached.
 */

template <typename Function,
          typename Argument1,
          typename Argument2,
          typename Argument3,
          typename Result>
class cached_ternary_function_t:
        public cached_function_t
            <cached_ternary_function_t<Function, Argument1,
                                       Argument2, Argument3,
                                       Result>,
             typename boost::tuple<Argument1, Argument2, Argument3>,
             Result>
{
public:
    // Some useful typedefs

    typedef Function  function_t;
    typedef Argument1  first_argument_type;
    typedef Argument2  second_argument_type;
    typedef Argument3  third_argument_type;
    typedef Result  result_type;

    /**
     * The cache key for the function's cache.  This is the function's
     * argument types, boxed up into a tuple.
     */

    typedef typename boost::tuple<Argument1, Argument2, Argument3>
    cache_key_t;

    /**
     * Unbox the cache key and call the underlying function.
     */

    result_type
    call(const cache_key_t &input)
    {
        return static_cast<Function *>(this)->
            call(boost::get<0>(input),
                 boost::get<1>(input),
                 boost::get<2>(input));
    }

    /**
     * Call the underlying function, if necessary; return the result
     * from our cache if we've already calculated it.
     */

    result_type
    operator () (const first_argument_type &arg1,
                 const second_argument_type &arg2,
                 const third_argument_type &arg3)
    {
        return cached_call(boost::make_tuple(arg1, arg2, arg3));
    }
};


} // namespace bdd
} // namespace ip

#endif /* IP__BDD_CACHE_HH */
