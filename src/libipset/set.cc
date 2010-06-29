/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <boost/cstdint.hpp>
#include <glog/logging.h>

#include <ip/ip.hh>
#include <ip/set.hh>
#include <ip/bdd/nodes.hh>
#include <ip/bdd/engine.hh>


using namespace ip::bdd;


namespace ip {


static bool_engine_t  engine;


set_t::set_t():
    bdd(engine.false_node())
{
}


bool
set_t::empty() const
{
    // Since BDDs are unique, the only empty set is the “false” BDD.

    return (bdd == engine.false_node());
}


node_id_t
set_t::create_ipv4_bdd
(const boost::uint8_t *addr, int netmask)
{
    // Special case — the BDD for a netmask that's out of range never
    // evaluates to true.

    if ((netmask <= 0) || (netmask > ipv4_addr_t::bit_size))
    {
        return engine.false_node();
    }

    // Otherwise build up the chain of BDD nodes for this IP address
    // or network.  Instead of using the Boolean operators, we
    // construct the BDD nodes by hand.

    if (netmask == ipv4_addr_t::bit_size)
    {
        DVLOG(2) << "Creating BDD for " << ipv4_addr_t(addr);
    } else {
        DVLOG(2) << "Creating BDD for " << ipv4_addr_t(addr)
                 << "/" << netmask;
    }

    // The end of the BDD node chain is the TRUE terminal, indicating
    // that the address is in the set.

    node_id_t  result = engine.true_node();
    node_id_t  false_node = engine.false_node();

    // Since the BDD needs to be ordered, we have to iterate through
    // the IP address's bits in reverse order.

    for (variable_t i = netmask; i > 0; i--)
    {
        variable_t  var = i - 1;  // need this b/c variable_t is unsigned
        int  byte_index = var / 8;
        int  bit_number = var % 8;
        int  bit_mask = 0x80 >> bit_number;

        if ((addr[byte_index] & bit_mask) == 0)
        {
            // This bit is not set in the IP address.
            result = engine.nonterminal(var, result, false_node);
        } else {
            // This bit is set in the IP address.
            result = engine.nonterminal(var, false_node, result);
        }
    }

    DVLOG(2) << "BDD is " << result;

    return result;
}


node_id_t
set_t::create_ipv6_bdd
(const boost::uint8_t *addr, int netmask)
{
    // Special case — the BDD for a netmask that's out of range never
    // evaluates to true.

    if ((netmask <= 0) || (netmask > ipv6_addr_t::bit_size))
    {
        return engine.false_node();
    }

    // Otherwise build up the chain of BDD nodes for this IP address
    // or network.  Instead of using the Boolean operators, we
    // construct the BDD nodes by hand.

    if (netmask == ipv6_addr_t::bit_size)
    {
        DVLOG(2) << "Creating BDD for " << ipv6_addr_t(addr);
    } else {
        DVLOG(2) << "Creating BDD for " << ipv6_addr_t(addr)
                 << "/" << netmask;
    }

    // The end of the BDD node chain is the TRUE terminal, indicating
    // that the address is in the set.

    node_id_t  result = engine.true_node();
    node_id_t  false_node = engine.false_node();

    // Since the BDD needs to be ordered, we have to iterate through
    // the IP address's bits in reverse order.

    for (variable_t i = netmask; i > 0; i--)
    {
        variable_t  var = i - 1;  // need this b/c variable_t is unsigned
        int  byte_index = var / 8;
        int  bit_number = var % 8;
        int  bit_mask = 0x80 >> bit_number;

        if ((addr[byte_index] & bit_mask) == 0)
        {
            // This bit is not set in the IP address.
            result = engine.nonterminal(var, result, false_node);
        } else {
            // This bit is set in the IP address.
            result = engine.nonterminal(var, false_node, result);
        }
    }

    DVLOG(2) << "BDD is " << result;

    return result;
}


bool
set_t::add_ipv4(const boost::uint8_t *addr, int netmask)
{
    // First, construct the BDD that represents this IP address —
    // i.e., where each boolean variable is assigned TRUE or FALSE
    // depending on whether the corresponding bit is set in the
    // address.

    node_id_t  addr_bdd = create_ipv4_bdd(addr, netmask);

    // Add addr to the set by constructing the logical OR of the old
    // set and the new element's BDD.

    DVLOG(2) << "Adding new BDD " << addr_bdd
             << " to set BDD " << bdd;
    node_id_t  new_bdd = engine.apply_or(addr_bdd, bdd);
    DVLOG(2) << "Result BDD is " << new_bdd;

    // If the BDD representing the set hasn't changed, then the
    // element was already in the set.

    bool  elem_already_present = new_bdd == bdd;

    // Store the set's new BDD node before returning.

    bdd = new_bdd;
    return elem_already_present;
}


bool
set_t::add_ipv6(const boost::uint8_t *addr, int netmask)
{
    // First, construct the BDD that represents this IP address —
    // i.e., where each boolean variable is assigned TRUE or FALSE
    // depending on whether the corresponding bit is set in the
    // address.

    node_id_t  addr_bdd = create_ipv6_bdd(addr, netmask);

    // Add addr to the set by constructing the logical OR of the old
    // set and the new element's BDD.

    DVLOG(2) << "Adding new BDD " << addr_bdd
             << " to set BDD " << bdd;
    node_id_t  new_bdd = engine.apply_or(addr_bdd, bdd);
    DVLOG(2) << "Result BDD is " << new_bdd;

    // If the BDD representing the set hasn't changed, then the
    // element was already in the set.

    bool  elem_already_present = new_bdd == bdd;

    // Store the set's new BDD node before returning.

    bdd = new_bdd;
    return elem_already_present;
}


std::ostream &
operator << (std::ostream &stream, const set_t &set)
{
    stream << "<set " << set.bdd << ">";
    return stream;
}


} // namespace ip
