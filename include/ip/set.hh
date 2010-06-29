/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__SET_HH
#define IP__SET_HH


#include <boost/cstdint.hpp>

#include <ip/ip.hh>
#include <ip/bdd/engine.hh>


namespace ip {


/**
 * A set of IP addresses.
 */

class set_t
{
private:
    /**
     * Create a BDD for an IPv4 address or family of IP addresses.
     * The corresponding BDD will have each boolean variable set if
     * the corresponding bit is set in the IP address.  The netmask
     * parameter can be used to limit the number of bits to constrain;
     * if this is less than ipv4_addr_t::bit_size, then an entire
     * network will be added to the set.
     */

    ip::bdd::node_id_t
    create_ipv4_bdd(const boost::uint8_t *addr, int netmask);

    /**
     * Create a BDD for an IPv6 address or family of IP addresses.
     * The corresponding BDD will have each boolean variable set if
     * the corresponding bit is set in the IP address.  The netmask
     * parameter can be used to limit the number of bits to constrain;
     * if this is less than ipv4_addr_t::bit_size, then an entire
     * network will be added to the set.
     */

    ip::bdd::node_id_t
    create_ipv6_bdd(const boost::uint8_t *addr, int netmask);

    /**
     * The BDD node that represents this set.
     */

    ip::bdd::node_id_t  bdd;

public:
    /**
     * Create a new, empty IP set.
     */

    set_t();

    /**
     * Create a new IP set that is a copy of another.
     */

    set_t(const set_t &other):
        bdd(other.bdd)
    {
    }

    /**
     * Assign the contents of another IP set into this one.
     */

    set_t &
    operator = (const set_t &other)
    {
        bdd = other.bdd;
        return (*this);
    }

    /**
     * Swap two IP sets.
     */

    void
    swap(set_t &other)
    {
        std::swap(bdd, other.bdd);
    }

    /**
     * Return whether two IP sets are equal.
     */

    bool
    operator == (const set_t &other) const
    {
        // Since BDDs are unique, sets can only be equal if their BDDs
        // are equal.

        return (bdd == other.bdd);
    }

    /**
     * Return whether two IP sets are not equal.
     */

    bool
    operator != (const set_t &other) const
    {
        // Since BDDs are unique, sets can only be equal if their BDDs
        // are equal.

        return (bdd != other.bdd);
    }

    /**
     * Return whether the IP set is empty.
     */

    bool
    empty() const;

    /**
     * Add an IPv4 address or network to an IP set.  We don't care
     * what specific type is used to represent the address; elem
     * should be a pointer to an address stored as a 32-bit big-endian
     * integer.  If netmask is not given, it defaults to 32 — i.e., a
     * single IP address.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add_ipv4(const boost::uint8_t *addr,
             int netmask = ipv4_addr_t::bit_size);

    /**
     * Add an IPv4 address or network to an IP set, based on an
     * address stored in an ipv4_addr_t.  If netmask is not given, it
     * defaults to 32 — i.e., a single IP address.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add(const ipv4_addr_t &addr,
        int netmask = ipv4_addr_t::bit_size)
    {
        return add_ipv4(static_cast<const boost::uint8_t *>(addr),
                        netmask);
    }

    /**
     * Add an IPv6 address or network to an IP set.  We don't care
     * what specific type is used to represent the address; elem
     * should be a pointer to an address stored as a 32-bit big-endian
     * integer.  If netmask is not given, it defaults to 32 — i.e., a
     * single IP address.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add_ipv6(const boost::uint8_t *addr,
             int netmask = ipv6_addr_t::bit_size);

    /**
     * Add an IPv6 address or network to an IP set, based on an
     * address stored in an ipv6_addr_t.  If netmask is not given, it
     * defaults to 32 — i.e., a single IP address.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add(const ipv6_addr_t &addr,
        int netmask = ipv6_addr_t::bit_size)
    {
        return add_ipv6(static_cast<const boost::uint8_t *>(addr),
                        netmask);
    }


    friend
    std::ostream &
    operator << (std::ostream &stream, const set_t &set);
};


std::ostream &
operator << (std::ostream &stream, const set_t &set);


} // namespace ip

#endif /* IP__SET_HH */
