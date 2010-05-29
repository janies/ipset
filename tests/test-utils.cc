/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */


#include <iostream>
#include <sstream>

#define __STDC_CONSTANT_MACROS
#include <boost/cstdint.hpp>

// Both of the following try to define CHECK.  We want the one from
// UnitTest.
#include <glog/logging.h>
#undef CHECK
#include <UnitTest++.h>

#include <ip/utils.hh>

using namespace ip;


TEST(Hex_8)
{
    std::cerr << "Starting Hex_8 test case." << std::endl;

    boost::uint8_t  val = 0xa8;

    CHECK_EQUAL("0xa8", to_hex(static_cast<uint16_t>(val), 2));
}


TEST(Hex_16)
{
    std::cerr << "Starting Hex_16 test case." << std::endl;

    boost::uint16_t  val = 0xa8;

    CHECK_EQUAL("0x00a8", to_hex(val, 4));
}


TEST(Hex_32)
{
    std::cerr << "Starting Hex_32 test case." << std::endl;

    boost::uint32_t  val = 0xa8;

    CHECK_EQUAL("0x000000a8", to_hex(val, 8));
}


//--------------------------------------------------------------------
// Boilerplate

int main(int argc, char **argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
    return UnitTest::RunAllTests();
}
