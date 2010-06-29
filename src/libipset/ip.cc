/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdio.h>

#include <cstring>
#include <ostream>

#include <boost/cstdint.hpp>

#include <ip/ip.hh>


namespace ip {


void
print_ipv4(std::ostream &stream, const boost::uint8_t *addr)
{
    stream << (int) addr[0] << "."
           << (int) addr[1] << "."
           << (int) addr[2] << "."
           << (int) addr[3];
}


std::ostream &
operator << (std::ostream &stream, const ipv4_addr_t &ip)
{
    print_ipv4(stream, static_cast<const boost::uint8_t *>(ip));
    return stream;
}


/**
 * For consistency, we use our own inet_ntop6 implementation and just
 * copy the result into the output stream.
 */

#define NS_IN6ADDRSZ  16
#define NS_INT16SZ     2

void
print_ipv6(std::ostream &stream, const boost::uint8_t *src)
{
    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    bool  ipv4_encapsulated = false;
    char tmp[sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"], *tp;
    struct { int base, len; } best, cur;
    unsigned int words[NS_IN6ADDRSZ / NS_INT16SZ];
    int i;

    /*
     * Preprocess:
     *      Copy the input (bytewise) array into a wordwise array.
     *      Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < NS_IN6ADDRSZ; i++)
        words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    best.len = 0;
    cur.base = -1;
    cur.len = 0;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;

    /*
     * Format the result.
     */
    tp = tmp;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        /* Are we inside the best run of 0x00's? */
        if (best.base != -1 && i >= best.base &&
            i < (best.base + best.len)) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0)
            *tp++ = ':';
        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 &&
            (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            ipv4_encapsulated = true;
            break;
        }
        tp += sprintf(tp, "%x", words[i]);
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) ==
        (NS_IN6ADDRSZ / NS_INT16SZ))
        *tp++ = ':';
    *tp++ = '\0';

    /*
     * This is where we differ from the original inet_ntop6; instead
     * of copying the result into an output buffer, we send it to the
     * stream.
     */

    if (ipv4_encapsulated)
    {
        stream << tmp
               << (int) src[12] << "."
               << (int) src[13] << "."
               << (int) src[14] << "."
               << (int) src[15];
    } else {
        stream << tmp;
    }
}


std::ostream &
operator << (std::ostream &stream, const ipv6_addr_t &ip)
{
    print_ipv6(stream, static_cast<const boost::uint8_t *>(ip));
    return stream;
}


struct stream_visitor: public boost::static_visitor<std::ostream &>
{
    std::ostream  &stream;

    stream_visitor(std::ostream &stream_): stream(stream_) {}

    std::ostream &
    operator () (const ipv4_addr_t &ipv4) const
    {
        return stream << ipv4;
    }

    std::ostream &
    operator () (const ipv6_addr_t &ipv6) const
    {
        return stream << ipv6;
    }
};


std::ostream &
operator << (std::ostream &stream, const ip_addr_t &ip)
{
    return boost::apply_visitor(stream_visitor(stream), ip.addr);
}


} // namespace ip
