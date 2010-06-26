/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <ostream>

#include <ip/bdd/nodes.hh>

namespace ip {
namespace bdd {


std::ostream &
operator << (std::ostream &stream, const node_t &node)
{
    stream << "nonterminal("
           << node.variable() << ","
           << node.low() << ","
           << node.high() << ")";

    return stream;
}


} // namespace bdd
} // namespace ip
