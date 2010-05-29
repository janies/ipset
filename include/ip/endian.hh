/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__ENDIAN_HH
#define IP__ENDIAN_HH


#include <exception>

#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>


namespace ip {
namespace endian {

enum endianness_t
{
    LITTLE,
    BIG
};

class cannot_swap_endian: public std::exception
{
public:
    virtual const char* what() const throw()
    {
        return "Cannot endian-swap this type";
    }
};

/**
 * Endian-swap an integer value.
 */

template <typename T>
inline T swap_endian(T in)
{
    throw cannot_swap_endian();
}

template <>
inline boost::uint8_t swap_endian<boost::uint8_t>(boost::uint8_t in)
{
    return in;
}

template <>
inline boost::uint16_t swap_endian<boost::uint16_t>(boost::uint16_t in)
{
    return
        (((in & 0xff00u) >> 8) |
         ((in & 0x00ffu) << 8));
}

template <>
inline boost::uint32_t swap_endian<boost::uint32_t>(boost::uint32_t in)
{
    return
        (((in & 0xff000000u) >> 24) |
         ((in & 0x00ff0000u) >>  8) |
         ((in & 0x0000ff00u) <<  8) |
         ((in & 0x000000ffu) << 24));
}

template <>
inline boost::uint64_t swap_endian<boost::uint64_t>(boost::uint64_t in)
{
    return
        (((in & 0xff00000000000000llu) >> 56) |
         ((in & 0x00ff000000000000llu) >> 40) |
         ((in & 0x0000ff0000000000llu) >> 24) |
         ((in & 0x000000ff00000000llu) >>  8) |
         ((in & 0x00000000ff000000llu) <<  8) |
         ((in & 0x0000000000ff0000llu) << 24) |
         ((in & 0x000000000000ff00llu) << 40) |
         ((in & 0x00000000000000ffllu) << 56));
}


/**
 * Return the endianness of the host.
 */

inline endianness_t
host_endianness()
{
#if defined BOOST_LITTLE_ENDIAN
    return LITTLE;
#elif defined BOOST_BIG_ENDIAN
    return BIG;
#else
#   error "Unknown endianness"
#endif
}


/**
 * Return the opposite endianness of the host.
 */

inline endianness_t
opposite_endianness()
{
#if defined BOOST_LITTLE_ENDIAN
    return BIG;
#elif defined BOOST_BIG_ENDIAN
    return LITTLE;
#else
#   error "Unknown endianness"
#endif
}


/**
 * Return whether the host is big-endian.
 */

inline bool
is_big()
{
    return host_endianness() == BIG;
}


/**
 * Return whether the host is little-endian.
 */

inline bool
is_little()
{
    return host_endianness() == LITTLE;
}


/**
 * Convert a host-endian integer into a big-endian integer.
 */

template <typename T>
inline T host_to_big(const T &in)
{
    if (is_little())
        return swap_endian(in);
    else
        return in;
}


/**
 * Convert a big-endian integer into a host-endian integer.
 */

template <typename T>
inline T big_to_host(const T &in)
{
    if (is_little())
        return swap_endian(in);
    else
        return in;
}


/**
 * Convert a host-endian integer into a little-endian integer.
 */

template <typename T>
inline T host_to_little(const T &in)
{
    if (is_big())
        return swap_endian(in);
    else
        return in;
}


/**
 * Convert a little-endian integer into a big-endian integer.
 */

template <typename T>
inline T little_to_host(const T &in)
{
    if (is_big())
        return swap_endian(in);
    else
        return in;
}


/**
 * Endian-swap an integer value in place.
 */

template <typename T>
inline void swap_endian_inplace(T &in)
{
    in = swap_endian(in);
}


/**
 * Convert a host-endian integer into a big-endian integer.
 */

template <typename T>
inline void host_to_big_inplace(T &in)
{
    if (is_little())
        swap_endian_inplace(in);
}


/**
 * Convert a big-endian integer into a host-endian integer.
 */

template <typename T>
inline void big_to_host_inplace(T &in)
{
    if (is_little())
        swap_endian_inplace(in);
}


/**
 * Convert a host-endian integer into a little-endian integer.
 */

template <typename T>
inline void host_to_little_inplace(T &in)
{
    if (is_big())
        swap_endian_inplace(in);
}


/**
 * Convert a little-endian integer into a big-endian integer.
 */

template <typename T>
inline void little_to_host_inplace(T &in)
{
    if (is_big())
        swap_endian_inplace(in);
}


} // namespace endian
} // namespace ip

#endif /* IP__ENDIAN_HH */
