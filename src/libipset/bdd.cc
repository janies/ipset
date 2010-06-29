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

#include <glog/logging.h>

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


node_id_t
node_cache_t::nonterminal_creator::call
(variable_t variable, node_id_t low, node_id_t high)
{
    node_t  node(variable, low, high);
    node_id_t  new_id = index_to_node_id(_nodes.size());
    DVLOG(3) << "Node " << new_id
             << " is now " << node;
    _nodes.push_back(node);
    return new_id;
}


} // namespace bdd
} // namespace ip
