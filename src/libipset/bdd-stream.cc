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

#include <boost/cstdint.hpp>
#include <boost/dynamic_bitset.hpp>
#include <glog/logging.h>

#include <ip/endian.hh>
#include <ip/bdd/nodes.hh>

namespace ip {
namespace bdd {


static const size_t  MAGIC_NUMBER_LENGTH = 8;

static const std::string
MAGIC_NUMBER("IP set\x00\x01", MAGIC_NUMBER_LENGTH);


template <typename T>
static void
output_uint8(std::ostream &stream, T val)
{
    boost::uint8_t  out_val = val;
    stream.write(reinterpret_cast<char *>(&out_val), sizeof(out_val));
}


template <typename T>
static void
output_uint32(std::ostream &stream, T val)
{
    boost::uint32_t  out_val = val;
    ip::endian::host_to_big_inplace(out_val);
    stream.write(reinterpret_cast<char *>(&out_val), sizeof(out_val));
}


template <typename T>
static void
output_uint64(std::ostream &stream, T val)
{
    boost::uint64_t  out_val = val;
    ip::endian::host_to_big_inplace(out_val);
    stream.write(reinterpret_cast<char *>(&out_val), sizeof(out_val));
}


node_id_t
node_cache_t::save_visit_node
(std::ostream &stream,
 std::vector<node_id_t> &serialized_ids,
 node_id_t &next_serialized_id,
 node_id_t id) const
{
    if (serialized_ids[node_id_to_index(id)] == 0)
    {
        const node_t  &node = this->node(id);
        DVLOG(3) << "Visiting node " << id << " " << node;

        // Output the node's nonterminal children before we output the
        // node itself.

        node_id_t  serialized_low, serialized_high;

        if (node.low() < 0)
        {
            serialized_low =
                save_visit_node(stream, serialized_ids,
                                next_serialized_id, node.low());
        } else {
            serialized_low = node.low();
        }

        if (node.high() < 0)
        {
            serialized_high =
                save_visit_node(stream, serialized_ids,
                                next_serialized_id, node.high());
        } else {
            serialized_high = node.high();
        }

        node_id_t  serialized_id = next_serialized_id--;
        DVLOG(3) << "Outputting node " << id
                 << " as serialized node " << serialized_id
                 << " = (" << node.variable()
                 << ',' << serialized_low
                 << ',' << serialized_high << ')';

        output_uint8(stream, node.variable());
        output_uint32(stream, serialized_low);
        output_uint32(stream, serialized_high);

        serialized_ids[node_id_to_index(id)] = serialized_id;
    }

    return serialized_ids[node_id_to_index(id)];
}


void
node_cache_t::save(std::ostream &stream, node_id_t id) const
{
    // First, output the magic number for an IP set.

    stream << MAGIC_NUMBER;

    if (id >= 0)
    {
        // If this is a terminal node, then we don't have to the
        // depth-first search.

        size_t  set_size =
            MAGIC_NUMBER_LENGTH +
            sizeof(boost::uint64_t) +   // length of set
            sizeof(boost::uint32_t) +   // number of nonterminals
            sizeof(boost::uint32_t);    // terminal value

        output_uint64(stream, set_size);
        output_uint32(stream, 0);
        output_uint32(stream, id);

        return;
    } else {
        // If this is a nonterminal node, first figure out how many
        // reachable nodes there are, to calculate the size of the
        // set.

        size_t  nonterminal_count = reachable_node_count(id);

        size_t  set_size =
            MAGIC_NUMBER_LENGTH +
            sizeof(boost::uint64_t) +   // length of set
            sizeof(boost::uint32_t) +   // number of nonterminals
            (nonterminal_count *
             (sizeof(boost::uint8_t) + sizeof(boost::uint32_t) * 2));

        output_uint64(stream, set_size);
        output_uint32(stream, nonterminal_count);

        // Since we only save the nodes that are actually used by the
        // set, the nonterminals will have different IDs in the
        // serialized set than in our node cache.  This stores a
        // mapping from cache IDs to serialized IDs.  An entry of 0
        // indicates that we haven't output this node to disk yet.

        std::vector<node_id_t>  serialized_ids(_nodes.size());
        node_id_t  next_serialized_id = -1;

        save_visit_node(stream, serialized_ids,
                        next_serialized_id, id);

        return;
    }
}


} // namespace bdd
} // namespace ip
