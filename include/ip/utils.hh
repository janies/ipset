/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IP__UTILS_HH
#define IP__UTILS_HH


#include <iomanip>
#include <sstream>
#include <string>


namespace ip {


/**
 * Converts a value to a hexadecimal string, using the "right" minimum
 * width for the size of the input.
 */

template <typename T>
inline std::string to_hex(const T &in, int min_width)
{
    std::ostringstream  ss;

    // We don't use std::showbase, because any zero-padding will be
    // placed to the left of the "0x" prefix.

    ss << "0x" << std::hex
       << std::setfill('0') << std::setw(min_width)
       << in;
    return ss.str();
}


} // namespace ip

#endif /* IP__UTILS_HH */
