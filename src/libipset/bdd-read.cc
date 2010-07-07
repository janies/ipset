/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <cstring>
#include <istream>
#include <vector>

#include <boost/cstdint.hpp>
#include <glog/logging.h>

#include <ip/endian.hh>
#include <ip/utils.hh>
#include <ip/bdd/nodes.hh>

namespace ip {
namespace bdd {


static const size_t  MAGIC_NUMBER_LENGTH = 6;

static const std::string
MAGIC_NUMBER("IP set", MAGIC_NUMBER_LENGTH);


template <typename T>
static bool
read_bytes(std::istream &stream, T *buf, std::streamsize length)
{
    DVLOG(3) << "Reading " << length << " bytes from stream.";
    stream.read(reinterpret_cast<char *>(buf), length);
    if (stream.gcount() < length)
    {
        LOG(ERROR) << "Only got " << stream.gcount() << " bytes.";
        return false;
    }

    return true;
}


template <typename T>
static bool
read_uint(std::istream &stream, T &val)
{
    T  in_val;
    if (read_bytes(stream, &in_val, sizeof(in_val)))
    {
        ip::endian::host_to_big_inplace(in_val);
        val = in_val;
        return true;
    } else {
        return false;
    }
}


template <typename T>
static bool
read_bytes_capped(std::istream &stream, size_t &cap,
                  T *buf, std::streamsize length)
{
    if (cap < sizeof(T))
    {
        LOG(ERROR) << "Need to read " << length << " bytes, "
                   << "but only " << cap << " are available.";
        return false;
    }

    DVLOG(3) << "Reading " << length << " bytes from stream.";
    stream.read(reinterpret_cast<char *>(buf), length);
    if (stream.gcount() < length)
    {
        LOG(ERROR) << "Only got " << stream.gcount() << " bytes.";
        return false;
    }

    cap -= length;
    return true;
}


template <typename T>
static bool
read_uint_capped(std::istream &stream, size_t &cap, T &val)
{
    T  in_val;
    if (read_bytes_capped(stream, cap, &in_val, sizeof(in_val)))
    {
        ip::endian::host_to_big_inplace(in_val);
        val = in_val;
        return true;
    } else {
        return false;
    }
}


bool
node_cache_t::load(std::istream &stream, node_id_t &id)
{
    // First, read in the magic number from the stream to ensure that
    // this is an IP set.

    char  magic[MAGIC_NUMBER_LENGTH];

    DVLOG(1) << "Reading IP set magic number.";

    if (!read_bytes(stream, magic, MAGIC_NUMBER_LENGTH))
        return false;

    if (std::memcmp(MAGIC_NUMBER.data(), magic,
                    MAGIC_NUMBER_LENGTH) != 0)
    {
        LOG(ERROR) << "Magic number doesn't match; "
                   << "this isn't an IP set.";
        return false;
    }

    // Read in the version number and dispatch to the right reading
    // function.

    boost::uint16_t  version;

    DVLOG(1) << "Reading IP set version number.";

    if (!read_uint(stream, version))
        return false;

    switch (version)
    {
      case 0x0001:
        return load_v1(stream, id);

      default:
        LOG(ERROR) << "Unknown IP set version "
                   << ip::to_hex(version, 4);
        return false;
    }
}


bool
node_cache_t::load_v1(std::istream &stream, node_id_t &id)
{
    DVLOG(1) << "Reading v1 IP set.";

    // We've already read in the magic number and version.  Next
    // should be the length of the encoded set.

    boost::uint64_t  length;
    DVLOG(2) << "Reading encoded length.";
    if (!read_uint(stream, length))
        return false;

    // The length includes the magic number, version number, and the
    // length field itself.  Remove those to get the cap on the
    // remaining stream.

    size_t  cap = length -
        MAGIC_NUMBER_LENGTH -
        sizeof(boost::uint16_t) -
        sizeof(length);

    DVLOG(2) << "Length cap is " << cap << " bytes.";

    // Read in the number of nonterminals.

    boost::uint32_t  nonterminal_count;
    DVLOG(2) << "Reading number of nonterminals.";
    if (!read_uint_capped(stream, cap, nonterminal_count))
        return false;

    // If there are no nonterminals, then there's only a single
    // terminal left to read.

    if (nonterminal_count == 0)
    {
        boost::uint32_t  terminal;
        DVLOG(2) << "Reading single terminal value.";
        if (!read_uint_capped(stream, cap, terminal))
            return false;

        // We should have reached the end of the encoded set.

        if (cap != 0)
        {
            LOG(ERROR) << "Malformed set: extra data at "
                       << "end of stream.";
            return false;
        }

        // Create a terminal node for this value and return it.

        id = this->terminal(terminal);
        return true;
    }

    // Otherwise, read in each nonterminal.  We need to keep track of
    // a mapping between each nonterminal's ID in the stream (which
    // are number consecutively from -1), and its ID in the node cache
    // (which could be anything).

    std::vector<node_id_t>  cache_ids(nonterminal_count);
    node_id_t  last_node_id = 0;

    for (unsigned int i = 0; i < nonterminal_count; i++)
    {
        // Each serialized node consists of a variable index, a low
        // pointer, and a high pointer.

        boost::uint8_t  variable;
        boost::uint32_t  low;
        boost::uint32_t  high;

        if (!read_uint_capped(stream, cap, variable))
            return false;
        if (!read_uint_capped(stream, cap, low))
            return false;
        if (!read_uint_capped(stream, cap, high))
            return false;

        node_id_t  serialized_low = static_cast<node_id_t>(low);
        node_id_t  serialized_high = static_cast<node_id_t>(high);

        DVLOG(2) << "Reading serialized node "
                 << index_to_node_id(i)
                 << " = (" << (int) variable
                 << ',' << serialized_low
                 << ',' << serialized_high << ')';

        // Turn the low pointer into a node ID.  If the pointer is >=
        // 0, it's a terminal value.  Otherwise, its a nonterminal ID,
        // indexing into the serialized nonterminal array.

        node_id_t  low_id;

        if (serialized_low >= 0)
        {
            low_id = this->terminal(serialized_low);
        } else {
            // The file format guarantees that any node reference
            // points to a node earlier in the serialized array.  That
            // means we can assume that cache_ids has already been
            // filled in for this node.

            low_id = cache_ids[node_id_to_index(serialized_low)];
            DVLOG(2) << "  Serialized ID " << serialized_low
                     << " is internal ID " << low_id;
        }

        // Do the same for the high pointer.

        node_id_t  high_id;

        if (serialized_high >= 0)
        {
            high_id = this->terminal(serialized_high);
        } else {
            // The file format guarantees that any node reference
            // points to a node earlier in the serialized array.  That
            // means we can assume that cache_ids has already been
            // filled in for this node.

            high_id = cache_ids[node_id_to_index(serialized_high)];
            DVLOG(2) << "  Serialized ID " << serialized_high
                     << " is internal ID " << high_id;
        }

        // Create a nonterminal node in the node cache.

        last_node_id = this->nonterminal(variable, low_id, high_id);

        DVLOG(2) << "Internal node " << last_node_id
                 << " = nonterminal(" << (int) variable
                 << ',' << low_id
                 << ',' << high_id << ')';

        // Remember the internal node ID for this new node, in case
        // any later serialized nodes point to it.

        cache_ids[i] = last_node_id;
    }

    // We should have reached the end of the encoded set.

    if (cap != 0)
    {
        LOG(ERROR) << "Malformed set: extra data at "
                   << "end of stream.";
        return false;
    }

    // The last node is the nonterminal for the entire set.

    id = last_node_id;
    return true;
}


} // namespace bdd
} // namespace ip
