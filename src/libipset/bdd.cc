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
#include <vector>

#include <boost/dynamic_bitset.hpp>
#include <boost/logic/tribool.hpp>
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


size_t
node_cache_t::reachable_node_count(node_id_t id) const
{
    // Create a bitset to track when we've visited a given node.

    boost::dynamic_bitset<>  visited(_nodes.size());

    // And a queue of nodes to check.

    std::vector<node_id_t>  queue;

    if (id < 0)
    {
        DVLOG(3) << "Adding node " << id << " to queue";
        queue.push_back(id);
    }

    // And somewhere to store the result.

    size_t  node_count = 0;

    // Check each node in turn.

    while (!queue.empty())
    {
        node_id_t  curr = queue.back();
        queue.pop_back();
        size_t  index = node_id_to_index(curr);

        // We don't have to do anything if this node is already in the
        // visited set.

        if (!visited.test(index))
        {
            DVLOG(3) << "Visiting node " << curr
                     << " for the first time.";

            // Add the node to the visited set.
            visited.set(index);

            // Increase the node count.
            node_count++;

            // And add the node's nonterminal children to the visit
            // queue.

            const node_t  &node = this->node(curr);

            if (node.low() < 0)
            {
                DVLOG(3) << "Adding node " << node.low()
                         << " to queue";
                queue.push_back(node.low());
            }

            if (node.high() < 0)
            {
                DVLOG(3) << "Adding node " << node.high()
                         << " to queue";
                queue.push_back(node.high());
            }
        }
    }

    // Return the result

    return node_count;
}


void node_cache_t::iterator::advance()
{
    // If we're already at the end of the iterator, don't do anything.

    if (finished)
        return;

    // We look at the last node in the stack.  If it's currently
    // assigned a false value, then we track down its true branch.  If
    // it's got a true branch, then we pop it off and check the next
    // to last node.

    while (!stack.empty())
    {
        boost::logic::tribool  &current_value =
            assignment()[stack.back().variable()];

        // The current value can't be indeterminate, because we
        // definitely assign a TRUE or FALSE to the variables of the
        // nodes that we encounter.

        if (current_value)
        {
            // We've checked both outgoing edges for this node, so pop
            // it off and look at its parent.

            stack.pop_back();

            // Before continuing, reset this node's variable to
            // indeterminate in the assignment.

            current_value = boost::logic::indeterminate;

        } else {
            // We've checked this node's low edge, but not its high
            // edge.  Set the variable to TRUE in the assignment, and
            // add the high edge's node to the node stack.

            current_value = true;
            add_node(stack.back().high());
            return;
        }
    }

    // If we fall through then we ran out of nodes to check.  That
    // means the iterator is done!

    finished = true;
}


void node_cache_t::iterator::add_node(node_id_t id)
{
    // Keep tracing down low edges until we reach a terminal.

    while (id < 0)
    {
        // Add this nonterminal node to the stack, and trace down
        // further into the tree.  We check low edges first, so set
        // the node's variable to FALSE in the assignment.

        const node_t  &node = cache.node(id);

        stack.push_back(node);
        assignment()[node.variable()] = false;

        id = node.low();
    }

    // Once we find a terminal node, save it away in the iterator
    // result and return.

    terminal() = id;
}


} // namespace bdd
} // namespace ip
