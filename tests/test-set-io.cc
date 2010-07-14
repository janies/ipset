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

// Both of the following try to define CHECK.  We want the one from
// UnitTest.
#include <glog/logging.h>
#undef CHECK
#include <UnitTest++.h>

#include <ip/ip.hh>
#include <ip/set.hh>

using namespace ip;


//--------------------------------------------------------------------
// Set data

static const char  *IPV4_SET_0_RAW =
        "IP set"                             // magic number
        "\x00\x01"                           // version
        "\x00\x00\x00\x00\x00\x00\x00\x18"   // length
        "\x00\x00\x00\x00"                   // node count
        "\x00\x00\x00\x00"                   // terminal value
    ;
static const size_t  IPV4_SET_0_LENGTH = 24;
static const std::string  IPV4_SET_0
(IPV4_SET_0_RAW, IPV4_SET_0_LENGTH);

static const char  *IPV4_SET_1_RAW =
    "IP set"                             // magic number
    "\x00\x01"                           // version
    "\x00\x00\x00\x00\x00\x00\x01\x3d"   // length
    "\x00\x00\x00\x21"                   // node count
    // node -1
    "\x20"                               // variable
    "\x00\x00\x00\x00"                   // low
    "\x00\x00\x00\x01"                   // high
    // node -2
    "\x1f"                               // variable
    "\xff\xff\xff\xff"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -3
    "\x1e"                               // variable
    "\xff\xff\xff\xfe"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -4
    "\x1d"                               // variable
    "\xff\xff\xff\xfd"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -5
    "\x1c"                               // variable
    "\xff\xff\xff\xfc"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -6
    "\x1b"                               // variable
    "\xff\xff\xff\xfb"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -7
    "\x1a"                               // variable
    "\xff\xff\xff\xfa"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -8
    "\x19"                               // variable
    "\xff\xff\xff\xf9"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -9
    "\x18"                               // variable
    "\xff\xff\xff\xf8"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -10
    "\x17"                               // variable
    "\xff\xff\xff\xf7"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -11
    "\x16"                               // variable
    "\xff\xff\xff\xf6"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -12
    "\x15"                               // variable
    "\xff\xff\xff\xf5"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -13
    "\x14"                               // variable
    "\xff\xff\xff\xf4"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -14
    "\x13"                               // variable
    "\xff\xff\xff\xf3"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -15
    "\x12"                               // variable
    "\xff\xff\xff\xf2"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -16
    "\x11"                               // variable
    "\xff\xff\xff\xf1"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -17
    "\x10"                               // variable
    "\xff\xff\xff\xf0"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -18
    "\x0f"                               // variable
    "\xff\xff\xff\xef"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -19
    "\x0e"                               // variable
    "\xff\xff\xff\xee"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -20
    "\x0d"                               // variable
    "\x00\x00\x00\x00"                   // low
    "\xff\xff\xff\xed"                   // high
    // node -21
    "\x0c"                               // variable
    "\xff\xff\xff\xec"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -22
    "\x0b"                               // variable
    "\x00\x00\x00\x00"                   // low
    "\xff\xff\xff\xeb"                   // high
    // node -23
    "\x0a"                               // variable
    "\xff\xff\xff\xea"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -24
    "\x09"                               // variable
    "\x00\x00\x00\x00"                   // low
    "\xff\xff\xff\xe9"                   // high
    // node -25
    "\x08"                               // variable
    "\xff\xff\xff\xe8"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -26
    "\x07"                               // variable
    "\xff\xff\xff\xe7"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -27
    "\x06"                               // variable
    "\xff\xff\xff\xe6"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -28
    "\x05"                               // variable
    "\xff\xff\xff\xe5"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -29
    "\x04"                               // variable
    "\xff\xff\xff\xe4"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -30
    "\x03"                               // variable
    "\xff\xff\xff\xe3"                   // low
    "\x00\x00\x00\x00"                   // high
    // node -31
    "\x02"                               // variable
    "\x00\x00\x00\x00"                   // low
    "\xff\xff\xff\xe2"                   // high
    // node -32
    "\x01"                               // variable
    "\x00\x00\x00\x00"                   // low
    "\xff\xff\xff\xe1"                   // high
    // node -33
    "\x00"                               // variable
    "\x00\x00\x00\x00"                   // low
    "\xff\xff\xff\xe0"                   // high
    ;
static const size_t  IPV4_SET_1_LENGTH = 317;
static const std::string  IPV4_SET_1
(IPV4_SET_1_RAW, IPV4_SET_1_LENGTH);


//--------------------------------------------------------------------
// Generic IP sets

TEST(IP_Set_Empty_Save)
{
    std::cerr << "Starting IP_Set_Empty_Save test case." << std::endl;

    set_t  s1;

    // Serialize the BDD into a string stream.

    std::ostringstream  ss;
    s1.save(ss);

    CHECK_EQUAL(UnitTest::binary_string(IPV4_SET_0),
                UnitTest::binary_string(ss.str()));
}


TEST(IP_Set_Empty_Load)
{
    std::cerr << "Starting IP_Set_Empty_Load test case." << std::endl;

    // Read the BDD from a string stream.

    std::istringstream  ss(IPV4_SET_0);

    set_t  s1;
    set_t  s2;

    CHECK(s2.load(ss));

    CHECK_EQUAL(s1, s2);
}


//--------------------------------------------------------------------
// IPv4 sets

TEST(IPv4_Set_Save_1)
{
    std::cerr << "Starting IPv4_Set_Save_1 test case." << std::endl;

    set_t  s1;

    ipv4_addr_t  a1(192,168,0,1);
    s1.add(a1);

    // Serialize the BDD into a string stream.

    std::ostringstream  ss;
    s1.save(ss);

    CHECK_EQUAL(UnitTest::binary_string(IPV4_SET_1),
                UnitTest::binary_string(ss.str()));
}

TEST(IPv4_Set_Load_1)
{
    std::cerr << "Starting IPv4_Set_Load_1 test case." << std::endl;

    // Read the BDD from a string stream.

    std::istringstream  ss(IPV4_SET_1);

    set_t  s1;
    set_t  s2;

    ipv4_addr_t  a1(192,168,0,1);
    s1.add(a1);

    CHECK(s2.load(ss));

    CHECK_EQUAL(s1, s2);
}


//--------------------------------------------------------------------
// Boilerplate

int main(int argc, char **argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
    return UnitTest::RunAllTests();
}
