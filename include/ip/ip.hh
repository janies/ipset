/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__IP_HH
#define IP__IP_HH


#include <cstring>
#include <ostream>

#include <boost/cstdint.hpp>
#include <boost/variant.hpp>

#include <ip/endian.hh>


namespace ip {


class ipv4_addr_t;
class ipv6_addr_t;
class ip_addr_t;


/**
 * A class representing an IPv4 address.  When piped to a
 * std::ostream, it will render using the standard dotted-quad
 * notation.
 *
 * NOTE: Since the class has no virtual methods, and only the single
 * uint32_t field, it takes up exactly four bytes, and can be used in
 * data structures to represent an IPv4 address.
 */

class ipv4_addr_t
{
public:
    /**
     * The number of bits in an IPv4 address.
     */

    static const int bit_size = 32;

    /**
     * Create a new, empty IPv4 address.
     */

    ipv4_addr_t():
        values(0)
    {
    }

    /**
     * Create a new IPv4 address from a raw data buffer.  The address
     * is expected to be in network-endian (i.e., big-endian) order.
     */

    ipv4_addr_t(const boost::uint8_t *buf):
        values(0)
    {
        std::memcpy(&values, buf, sizeof(values));
    }

    /**
     * Create a new IPv4 address from a dotted-quad notation.  For
     * instance, ipv4_addr_t(192,168,0,1) yields the IP address
     * 192.168.0.1.
     */

    ipv4_addr_t(boost::uint8_t o0,
                boost::uint8_t o1,
                boost::uint8_t o2,
                boost::uint8_t o3):
        values(make_from_octets(o0, o1, o2, o3))
    {
    }

    /**
     * Create a new IPv4 address from a 4-byte integer.  The integer
     * is expected to be in network-endian (i.e., big-endian) order.
     * If the integer is host-endian, you can use the ip::big_endian
     * function to swap it.
     */

    explicit
    ipv4_addr_t(boost::uint32_t u32_):
        values(u32_)
    {
    }

    /**
     * Create a new IPv4 address as a copy of another address.
     */

    ipv4_addr_t(const ipv4_addr_t &other):
        values(other.values)
    {
    }

    /**
     * Assign another IPv4 address into this one.
     */

    ipv4_addr_t &
    operator = (const ipv4_addr_t &other)
    {
        values = other.values;
        return (*this);
    }

    /**
     * Swap two IPv4 addresses.
     */

    void
    swap(ipv4_addr_t &other)
    {
        std::swap(values, other.values);
    }

    /**
     * Translate the IP address into a host-endian 32-bit integer.
     */

    boost::uint32_t host_endian() const
    {
        return ip::endian::big_to_host(values.u32);
    }

    /**
     * Return a particular octet of the IP address.  Octets are
     * numbered 0-3.  Octet 0 is the most significant octet — the one
     * which is listed first in the dotted-quad notation, and stored
     * first in memory.
     */

    boost::uint8_t octet(int idx) const
    {
        return values.u8[idx];
    }

    /**
     * The IP address can be cast to a (uint8_t *); this just returns
     * a pointer to the raw address data.
     */

    operator const boost::uint8_t* () const
    {
        return values.u8;
    }

    /**
     * Test two IP addresses for equality.
     */

    bool operator == (const ipv4_addr_t &other) const
    {
        return values.u32 == other.values.u32;
    }

    /**
     * Test two IP addresses for equality.
     */

    bool operator == (const ipv6_addr_t &other) const
    {
        // An IPv4 can never be equal to an IPv6 address.
        return false;
    }

    /**
     * Compare two IP addresses.
     */

    bool operator < (const ipv4_addr_t &other) const
    {
        return values.u32 < other.values.u32;
    }

private:

    /**
     * A union type that allows us to easily access the IP address as
     * both a 32-bit integer or as four 8-bit integers.
     */

    union union_t
    {
        boost::uint32_t  u32;
        boost::uint8_t  u8[4];

        union_t(boost::uint32_t u32_): u32(u32_) {}
    };

    /**
     * This is the only field in the class.  That way it takes up 4
     * bytes, and can be used in structs to represent address fields.
     */

    union_t  values;

    /**
     * Fill in the octet portion of the union type, then return the
     * result.  This is a helper method for the four-octet ipv4_addr_t
     * constructor; when the ipv4_addr_t is constant, we can't fill in
     * the octets in the constructor body.  We can call this function,
     * though, in the initializer.
     */

    static boost::uint32_t
    make_from_octets(boost::uint8_t o0,
                     boost::uint8_t o1,
                     boost::uint8_t o2,
                     boost::uint8_t o3)
    {
        union_t  result(0);
        result.u8[0] = o0;
        result.u8[2] = o2;
        result.u8[1] = o1;
        result.u8[3] = o3;
        return result.u32;
    }

};


void
print_ipv4(std::ostream &stream, const boost::uint8_t *addr);

std::ostream &
operator << (std::ostream &stream, const ipv4_addr_t &ip);


/**
 * A class representing an IPv6 address.  When piped to a
 * std::ostream, it will render using the standard colon-hex notation.
 *
 * NOTE: Since the class has no virtual methods, and only the single
 * uint32_t array, it takes up exactly 16 bytes, and can be used in
 * data structures to represent an IPv6 address.
 */

class ipv6_addr_t
{
public:
    /**
     * The number of bits in an IPv6 address.
     */

    static const int bit_size = 128;

    /**
     * Create a new, empty IPv6 address.
     */

    ipv6_addr_t():
        values()
    {
    }

    /**
     * Create a new IPv6 address from a raw data buffer.  The address
     * is expected to be in network-endian (i.e., big-endian) order.
     */

    ipv6_addr_t(const boost::uint8_t *buf):
        values()
    {
        std::memcpy(&values, buf, sizeof(values));
    }

    /**
     * Create a new IPv6 address from four 32-bit integers.  The
     * integers are expected to be in network-endian order.
     */

    ipv6_addr_t(boost::uint32_t dw0,
                boost::uint32_t dw1,
                boost::uint32_t dw2,
                boost::uint32_t dw3):
        values(make_from_dwords(dw0, dw1, dw2, dw3))
    {
    }

    /**
     * Create a new IPv6 address from a colon-hex notation.  For
     * instance, ipv6_addr_t(0xfe80,0,0,0,0,0,0,1) yields the IP
     * address fe80::1.  Each word is expected to be in host-endian
     * order; they will be automatically converted to big-endian
     * internally.
     */

    ipv6_addr_t(boost::uint16_t w0,
                boost::uint16_t w1,
                boost::uint16_t w2,
                boost::uint16_t w3,
                boost::uint16_t w4,
                boost::uint16_t w5,
                boost::uint16_t w6,
                boost::uint16_t w7):
        values(make_from_words(w0, w1, w2, w3, w4, w5, w6, w7))
    {
    }

    /**
     * Create a new IPv6 address as a copy of another address.
     */

    ipv6_addr_t(const ipv6_addr_t &other):
        values(other.values)
    {
    }

    /**
     * Assign another IPv6 address into this one.
     */

    ipv6_addr_t &
    operator = (const ipv6_addr_t &other)
    {
        values = other.values;
        return (*this);
    }

    /**
     * Swap two IPv6 addresses.
     */

    void
    swap(ipv6_addr_t &other)
    {
        std::swap(values, other.values);
    }

    /**
     * Return a particular octet of the IP address.  Octets are
     * numbered 0-15.  Octet 0 is the most significant octet — the one
     * which is listed first in the dotted-quad notation, and stored
     * first in memory.
     */

    boost::uint8_t octet(int idx) const
    {
        return values.u8[idx];
    }

    /**
     * The IP address can be cast to a (uint8_t *); this just returns
     * a pointer to the raw address data.
     */

    operator const boost::uint8_t* () const
    {
        return values.u8;
    }

    /**
     * Test two IP addresses for equality.
     */

    bool operator == (const ipv6_addr_t &other) const
    {
        return ((values.u32[0] == other.values.u32[0]) &&
                (values.u32[1] == other.values.u32[1]) &&
                (values.u32[2] == other.values.u32[2]) &&
                (values.u32[3] == other.values.u32[3]));
    }

    /**
     * Test two IP addresses for equality.
     */

    bool operator == (const ipv4_addr_t &other) const
    {
        // An IPv4 can never be equal to an IPv6 address.
        return false;
    }

    /**
     * Compare two IP addresses.
     */

    bool operator < (const ipv6_addr_t &other) const
    {
        if (values.u32[0] < other.values.u32[0])
            return true;
        else if (values.u32[0] > other.values.u32[0])
            return false;

        if (values.u32[1] < other.values.u32[1])
            return true;
        else if (values.u32[1] > other.values.u32[1])
            return false;

        if (values.u32[2] < other.values.u32[2])
            return true;
        else if (values.u32[2] > other.values.u32[2])
            return false;

        if (values.u32[3] < other.values.u32[3])
            return true;
        else if (values.u32[3] > other.values.u32[3])
            return false;

        return false;
    }

    friend std::ostream &
    operator << (std::ostream &stream, const ipv6_addr_t &ip);

private:

    /**
     * A union type that allows us to easily access the IP address as
     * both as an array of 32-bit, 16-bit, or 8-bit integers.
     */

    union union_t
    {
        boost::uint32_t  u32[4];
        boost::uint16_t  u16[8];
        boost::uint8_t  u8[16];
    };

    /**
     * This is the only field in the class.  That way it takes up 16
     * bytes, and can be used in structs to represent address fields.
     */

    union_t  values;

    /**
     * Fill in the double-word portion of the union type, then return
     * the result.  This is a helper method for the four-dword
     * ipv6_addr_t constructor; when the ipv6_addr_t is constant, we
     * can't fill in the words in the constructor body.  We can call
     * this function, though, in the initializer.
     */

    static union_t
    make_from_dwords(boost::uint32_t dw0,
                     boost::uint32_t dw1,
                     boost::uint32_t dw2,
                     boost::uint32_t dw3)
    {
        union_t  result;
        result.u32[0] = dw0;
        result.u32[1] = dw1;
        result.u32[2] = dw2;
        result.u32[3] = dw3;
        return result;
    }

    /**
     * Fill in the word portion of the union type, then return the
     * result.  This is a helper method for the eight-word ipv6_addr_t
     * constructor; when the ipv6_addr_t is constant, we can't fill in
     * the words in the constructor body.  We can call this function,
     * though, in the initializer.
     */

    static union_t
    make_from_words(boost::uint16_t w0,
                    boost::uint16_t w1,
                    boost::uint16_t w2,
                    boost::uint16_t w3,
                    boost::uint16_t w4,
                    boost::uint16_t w5,
                    boost::uint16_t w6,
                    boost::uint16_t w7)
    {
        union_t  result;
        result.u16[0] = ip::endian::host_to_big(w0);
        result.u16[1] = ip::endian::host_to_big(w1);
        result.u16[2] = ip::endian::host_to_big(w2);
        result.u16[3] = ip::endian::host_to_big(w3);
        result.u16[4] = ip::endian::host_to_big(w4);
        result.u16[5] = ip::endian::host_to_big(w5);
        result.u16[6] = ip::endian::host_to_big(w6);
        result.u16[7] = ip::endian::host_to_big(w7);
        return result;
    }

};


void
print_ipv6(std::ostream &stream, const boost::uint8_t *addr);

std::ostream &
operator << (std::ostream &stream, const ipv6_addr_t &ip);


/**
 * A class representing an IPv4 or IPv6 address.  We use the Boost
 * Variant library to store the underlying address; unlike the
 * ipv4_addr_t and ipv6_addr_t types, it should not be included
 * directly into a struct that will read from or written to disk.
 */

class ip_addr_t
{
public:

    /**
     * Create a new, empty IP address.
     */

    ip_addr_t():
        addr()
    {
    }

    /**
     * Create a generic IP address from an IPv4 address.
     */

    ip_addr_t(const ipv4_addr_t &ipv4):
        addr(ipv4)
    {
    }

    /**
     * Create a generic IP address from an IPv6 address.
     */

    ip_addr_t(const ipv6_addr_t &ipv6):
        addr(ipv6)
    {
    }

    /**
     * Create a generic IP address from another generic IP address.
     */

    ip_addr_t(const ip_addr_t &other):
        addr(other.addr)
    {
    }

    /**
     * Assign another IPv4 address into this one.
     */

    ip_addr_t &
    operator = (const ipv4_addr_t &other)
    {
        addr = other;
        return (*this);
    }

    /**
     * Assign another IPv6 address into this one.
     */

    ip_addr_t &
    operator = (const ipv6_addr_t &other)
    {
        addr = other;
        return (*this);
    }

    /**
     * Assign another IP address into this one.
     */

    ip_addr_t &
    operator = (const ip_addr_t &other)
    {
        ip_addr_t  temp(other);
        swap(temp);
        return (*this);
    }

    /**
     * Swap two IP addresses.
     */

    void
    swap(ip_addr_t &other)
    {
        std::swap(addr, other.addr);
    }

    /**
     * Return the IP version of the address.
     */

    int version() const
    {
        return boost::apply_visitor(version_visitor(), addr);
    }

    /**
     * The IP address can be cast to a (uint8_t *); this just returns
     * a pointer to the raw address data.
     */

    operator const boost::uint8_t* () const
    {
        return boost::apply_visitor(raw_visitor(), addr);
    }

    /**
     * Test two IP addresses for equality.
     */

    bool operator == (const ip_addr_t &other) const
    {
        return addr == other.addr;
    }

    /**
     * Compare two IP addresses.
     */

    bool operator < (const ip_addr_t &other) const
    {
        return addr < other.addr;
    }

    friend
    std::ostream &
    operator << (std::ostream &stream, const ip_addr_t &ip);

private:

    /**
     * A handy typedef for referring to the Boost Variant type that we
     * use to store the actual address.
     */

    typedef boost::variant<ipv4_addr_t, ipv6_addr_t>  variant_t;

    /**
     * The contained IPv4 or IPv6 address.
     */

    variant_t  addr;

    /**
     * A Boost visitor that returns the type of address.
     */

    struct version_visitor: public boost::static_visitor<int>
    {
        int operator () (const ipv4_addr_t &ipv4) const { return 4; }
        int operator () (const ipv6_addr_t &ipv6) const { return 6; }
    };

    /**
     * A Boost visitor that returns a pointer to the address's data.
     */

    struct raw_visitor:
        public boost::static_visitor<const boost::uint8_t *>
    {
        const boost::uint8_t *
        operator () (const ipv4_addr_t &ipv4) const
        {
            return (const boost::uint8_t *) ipv4;
        }

        const boost::uint8_t *
        operator () (const ipv6_addr_t &ipv6) const
        {
            return (const boost::uint8_t *) ipv6;
        }
    };

};


std::ostream &
operator << (std::ostream &stream, const ip_addr_t &ip);


/**
 * Test two IP addresses for equality.
 */

inline bool
operator == (const ipv4_addr_t &a, const ip_addr_t &b)
{
    return ip_addr_t(a) == b;
}


/**
 * Test two IP addresses for equality.
 */

inline bool
operator == (const ipv6_addr_t &a, const ip_addr_t &b)
{
    return ip_addr_t(a) == b;
}


} // namespace ip

#endif /* IP__IP_HH */
