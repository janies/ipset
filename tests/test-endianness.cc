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

#define __STDC_CONSTANT_MACROS
#include <boost/cstdint.hpp>

// Both of the following try to define CHECK.  We want the one from
// UnitTest.
#include <glog/logging.h>
#undef CHECK
#include <UnitTest++.h>

#include <ip/endian.hh>

using namespace ip::endian;


TEST(Big_To_Host_16)
{
    std::cerr << "Starting Big_To_Host_16 test case." << std::endl;

    boost::uint8_t  raw[] = { 0x01, 0x02 };
    boost::uint16_t  *input = (boost::uint16_t *) raw;
    boost::uint16_t  expected = 0x0102;

    CHECK_EQUAL(expected, big_to_host(*input));
}


TEST(Big_To_Host_32)
{
    std::cerr << "Starting Big_To_Host_32 test case." << std::endl;

    boost::uint8_t  raw[] = { 0x01, 0x02, 0x03, 0x04 };
    boost::uint32_t  *input = (boost::uint32_t *) raw;
    boost::uint32_t  expected = 0x01020304;

    CHECK_EQUAL(expected, big_to_host(*input));
}


TEST(Big_To_Host_64)
{
    std::cerr << "Starting Big_To_Host_64 test case." << std::endl;

    boost::uint8_t  raw[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };
    boost::uint64_t  *input = (boost::uint64_t *) raw;
    boost::uint64_t  expected = UINT64_C(0x0102030405060708);

    CHECK_EQUAL(expected, big_to_host(*input));
}


TEST(Little_To_Host_16)
{
    std::cerr << "Starting Little_To_Host_16 test case." << std::endl;

    boost::uint8_t  raw[] = { 0x01, 0x02 };
    boost::uint16_t  *input = (boost::uint16_t *) raw;
    boost::uint16_t  expected = 0x0201;

    CHECK_EQUAL(expected, little_to_host(*input));
}


TEST(Little_To_Host_32)
{
    std::cerr << "Starting Little_To_Host_32 test case." << std::endl;

    boost::uint8_t  raw[] = { 0x01, 0x02, 0x03, 0x04 };
    boost::uint32_t  *input = (boost::uint32_t *) raw;
    boost::uint32_t  expected = 0x04030201;

    CHECK_EQUAL(expected, little_to_host(*input));
}


TEST(Little_To_Host_64)
{
    std::cerr << "Starting Little_To_Host_64 test case." << std::endl;

    boost::uint8_t  raw[] = {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
    };
    boost::uint64_t  *input = (boost::uint64_t *) raw;
    boost::uint64_t  expected = UINT64_C(0x0807060504030201);

    CHECK_EQUAL(expected, little_to_host(*input));
}


int main(int argc, char **argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
    return UnitTest::RunAllTests();
}
