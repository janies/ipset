/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <functional>

#include <ip/bdd/operators.hh>
#include <ip/bdd/operators-int.hh>


namespace ip {
namespace bdd {

template <>
struct operator_name<std::logical_and<bool> >
{
    static const std::string op_name()
    {
        return "AND";
    }
};


template <>
struct operator_name<std::logical_or<bool> >
{
    static const std::string op_name()
    {
        return "OR";
    }
};


template class binary_operator<std::logical_and<bool> >;
template class binary_operator<std::logical_or<bool> >;


} // namespace bdd
} // namespace ip
