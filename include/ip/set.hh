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


#include <istream>
#include <ostream>

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
     * Return the BDD variable number for the given bit of an IPv4
     * address.  IPv4 addresses use variables 1-32.  (Variable 0 is
     * used to identify the kind of address — TRUE for IPv4, FALSE for
     * IPv6.)
     */

    static ip::bdd::variable_t
    ipv4_var(int index)
    {
        return index + 1;
    }

    /**
     * Return the BDD variable number for the given bit of an IPv6
     * address.  IPv6 addresses use variables 1-128.  (Variable 0 is
     * used to identify the kind of address — TRUE for IPv4, FALSE for
     * IPv6.)
     */

    static ip::bdd::variable_t
    ipv6_var(int index)
    {
        return index + 1;
    }

    /**
     * Return the bit index for a given IPv4 BDD variable.
     */

    static int
    ipv4_index(ip::bdd::variable_t var)
    {
        return var - 1;
    }

    /**
     * Return the bit index for a given IPv6 BDD variable.
     */

    static int
    ipv6_index(ip::bdd::variable_t var)
    {
        return var - 1;
    }

    /**
     * A helper class that allows an IPv4 byte array to be indexed by
     * BDD variable rather than by byte.
     */

    struct ipv4_var_indexed;

    /**
     * A helper class that allows an IPv6 byte array to be indexed by
     * BDD variable rather than by byte.
     */

    struct ipv6_var_indexed;

    /**
     * Create a BDD for an IPv4 address or family of IP addresses.
     * The corresponding BDD will have each boolean variable set if
     * the corresponding bit is set in the IP address.  The netmask
     * parameter can be used to limit the number of bits to constrain;
     * if this is less than ipv4_addr_t::bit_size, then an entire
     * network will be added to the set.
     */

    ip::bdd::node_id_t
    create_ipv4_bdd(const boost::uint8_t *addr, unsigned int netmask);

    /**
     * Create a BDD for an IPv6 address or family of IP addresses.
     * The corresponding BDD will have each boolean variable set if
     * the corresponding bit is set in the IP address.  The netmask
     * parameter can be used to limit the number of bits to constrain;
     * if this is less than ipv4_addr_t::bit_size, then an entire
     * network will be added to the set.
     */

    ip::bdd::node_id_t
    create_ipv6_bdd(const boost::uint8_t *addr, unsigned int netmask);

    /**
     * A Boost variant visitor for adding a generic IP address to a
     * set.
     */

    struct add_ip_visitor;

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
             unsigned int netmask = ipv4_addr_t::bit_size);

    /**
     * Add an IPv4 address or network to an IP set, based on an
     * address stored in an ipv4_addr_t.  If netmask is not given, it
     * defaults to 32 — i.e., a single IP address.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add(const ipv4_addr_t &addr,
        unsigned int netmask = ipv4_addr_t::bit_size)
    {
        return add_ipv4
            (static_cast<const boost::uint8_t *>(addr), netmask);
    }

    /**
     * Add an IPv6 address or network to an IP set.  We don't care
     * what specific type is used to represent the address; elem
     * should be a pointer to an address stored as a 128-bit
     * big-endian integer.  If netmask is not given, it defaults to
     * 128 — i.e., a single IP address.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add_ipv6(const boost::uint8_t *addr,
             unsigned int netmask = ipv6_addr_t::bit_size);

    /**
     * Add an IPv6 address or network to an IP set, based on an
     * address stored in an ipv6_addr_t.  If netmask is not given, it
     * defaults to 128 — i.e., a single IP address.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add(const ipv6_addr_t &addr,
        unsigned int netmask = ipv6_addr_t::bit_size)
    {
        return add_ipv6
            (static_cast<const boost::uint8_t *>(addr), netmask);
    }

    /**
     * Add a generic IP address to an IP set, based on an address
     * stored in an ip_addr_t.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add(const ip_addr_t &addr);

    /**
     * Add a generic IP network to an IP set, based on an address
     * stored in an ip_addr_t.
     *
     * Return whether the value was already in the set or not.
     */

    bool
    add(const ip_addr_t &addr, unsigned int netmask);

    /**
     * Check whether a particular IPv4 address is in the set.  We
     * don't care what specific type is used to represent the address;
     * elem should be a pointer to an address stored as a 32-bit
     * big-endian integer.
     */

    bool
    contains_ipv4(const boost::uint8_t *addr);

    /**
     * Check whether a particular ipv4_addr_t is in the set.
     */

    bool
    contains(const ipv4_addr_t &addr)
    {
        return contains_ipv4
            (static_cast<const boost::uint8_t *>(addr));
    }

    /**
     * Check whether a particular IPv6 address is in the set.  We
     * don't care what specific type is used to represent the address;
     * elem should be a pointer to an address stored as a 128-bit
     * big-endian integer.
     */

    bool
    contains_ipv6(const boost::uint8_t *addr);

    /**
     * Check whether a particular ipv6_addr_t is in the set.
     */

    bool
    contains(const ipv6_addr_t &addr)
    {
        return contains_ipv6
            (static_cast<const boost::uint8_t *>(addr));
    }

    /**
     * Return the amount of memory used by this set.
     */

    size_t
    memory_size() const;

    /**
     * Load an IP set from an input stream.  This replaces the
     * contents of the current set.
     */

    bool
    load(std::istream &stream);

    /**
     * Persist an IP set to an output stream.
     */

    void
    save(std::ostream &stream) const;


    friend
    std::ostream &
    operator << (std::ostream &stream, const set_t &set);
};


std::ostream &
operator << (std::ostream &stream, const set_t &set);


} // namespace ip

#endif /* IP__SET_HH */
