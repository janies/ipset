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
// Generic IP sets

TEST(IP_Set_Empty)
{
    std::cerr << "Starting IP_Set_Empty test case." << std::endl;

    set_t  s1;

    CHECK(s1.empty());
}

TEST(IP_Set_Empty_Equal)
{
    std::cerr << "Starting IP_Set_Empty_Equal test case." << std::endl;

    set_t  s1;
    set_t  s2;

    CHECK_EQUAL(s1, s2);
}

TEST(IP_Set_v4_v6_Distinct)
{
    std::cerr << "Starting IP_Set_v4_v6_Distinct test case." << std::endl;

    // Verify that IPv4 addresses and IPv6 addresses don't overlap.

    set_t  s1;
    set_t  s2;

    // The first four bytes of a2 are the same as a1.
    ipv4_addr_t  a1(192,168,0,1);
    ipv6_addr_t  a2(0xc0a8,0x0001,0,0,0,0,0,0);

    s1.add(a1);
    s2.add(a2, 32);

    CHECK(s1 != s2);
}

TEST(IP_Set_v4_v6_Contains_1)
{
    std::cerr << "Starting IP_Set_v4_v6_Contains_1 test case." << std::endl;

    set_t  s1;

    ipv4_addr_t  a1(192,168,0,1);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1);

    CHECK(s1.contains(a1));
    CHECK(!s1.contains(a2));
}

TEST(IP_Set_v4_v6_Contains_2)
{
    std::cerr << "Starting IP_Set_v4_v6_Contains_2 test case." << std::endl;

    set_t  s1;

    ipv4_addr_t  a1(0,0,0,0);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1);

    CHECK(s1.contains(a1));
    CHECK(!s1.contains(a2));
}


//--------------------------------------------------------------------
// IPv4 sets

TEST(IPv4_Set_Nonempty)
{
    std::cerr << "Starting IPv4_Set_Nonempty test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1);

    CHECK(!s1.empty());
}

TEST(IPv4_Set_Nonempty_Net)
{
    std::cerr << "Starting IPv4_Set_Nonempty_Net test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1, 16);

    CHECK(!s1.empty());
}

TEST(IPv4_Set_Size_1)
{
    std::cerr << "Starting IPv4_Set_Size_1 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1);

    CHECK_EQUAL(396u, s1.memory_size());
}

TEST(IPv4_Set_Size_Net_1)
{
    std::cerr << "Starting IPv4_Set_Size_Net_1 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1, 16);

    CHECK_EQUAL(204u, s1.memory_size());
}

TEST(IPv4_Set_Size_2)
{
    std::cerr << "Starting IPv4_Set_Size_2 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(10,0,5,128);

    s1.add(a1);
    s1.add(a2);

    CHECK_EQUAL(768u, s1.memory_size());
}

TEST(IPv4_Set_Size_Net_2)
{
    std::cerr << "Starting IPv4_Set_Size_Net_2 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(10,0,5,128);

    s1.add(a1, 16);
    s1.add(a2, 16);

    CHECK_EQUAL(348u, s1.memory_size());
}

TEST(IPv4_Set_Contains_1)
{
    std::cerr << "Starting IPv4_Set_Contains_1 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1);

    CHECK(s1.contains(a1));
}

TEST(IPv4_Set_Contains_Net_1)
{
    std::cerr << "Starting IPv4_Set_Contains_Net_1 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1, 16);

    CHECK(s1.contains(a1));
}

TEST(IPv4_Set_Contains_2)
{
    std::cerr << "Starting IPv4_Set_Contains_2 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(10,0,5,128);

    s1.add(a1);
    s1.add(a2);

    CHECK(s1.contains(a1));
    CHECK(s1.contains(a2));
}

TEST(IPv4_Set_Contains_Net_2)
{
    std::cerr << "Starting IPv4_Set_Contains_Net_2 test case." << std::endl;

    set_t  s1;
    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(10,0,5,128);

    s1.add(a1, 16);
    s1.add(a2, 16);

    CHECK(s1.contains(a1));
    CHECK(s1.contains(a2));
}

TEST(IPv4_Set_Equal_1)
{
    std::cerr << "Starting IPv4_Set_Equal_1 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1);
    s2.add(a1);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv4_Set_Equal_Net_1)
{
    std::cerr << "Starting IPv4_Set_Equal_Net_1 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv4_addr_t  a1(192,168,0,1);

    s1.add(a1, 16);
    s2.add(a1, 16);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv4_Set_Equal_2)
{
    std::cerr << "Starting IPv4_Set_Equal_2 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(10,0,5,128);

    s1.add(a1);
    s1.add(a2);

    s2.add(a1);
    s2.add(a2);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv4_Set_Equal_Net_2)
{
    std::cerr << "Starting IPv4_Set_Equal_Net_2 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(10,0,5,128);

    s1.add(a1, 16);
    s1.add(a2, 16);

    s2.add(a1, 16);
    s2.add(a2, 16);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv4_Gen_Set_Equal_1)
{
    std::cerr << "Starting IPv4_Gen_Set_Equal_1 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv4_addr_t  a1(192,168,0,1);
    ip_addr_t  ga1(a1);

    s1.add(a1);
    s2.add(ga1);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv4_Set_Idempotent)
{
    std::cerr << "Starting IPv4_Set_Idempotent test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv4_addr_t  a1(192,168,0,1);

    CHECK(!s1.add(a1));
    CHECK(s1.add(a1));

    s2.add(a1);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv4_Set_Idempotent_Net)
{
    std::cerr << "Starting IPv4_Set_Idempotent_Net test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv4_addr_t  a1(192,168,0,1);

    CHECK(!s1.add(a1, 16));
    CHECK(s1.add(a1, 16));

    s2.add(a1, 16);

    CHECK_EQUAL(s1, s2);
}


//--------------------------------------------------------------------
// IPv6 sets

TEST(IPv6_Set_Nonempty)
{
    std::cerr << "Starting IPv6_Set_Nonempty test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1);

    CHECK(!s1.empty());
}

TEST(IPv6_Set_Nonempty_Net)
{
    std::cerr << "Starting IPv6_Set_Nonempty_Net test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1, 16);

    CHECK(!s1.empty());
}

TEST(IPv6_Set_Size_1)
{
    std::cerr << "Starting IPv6_Set_Size_1 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1);

    CHECK_EQUAL(1548u, s1.memory_size());
}

TEST(IPv6_Set_Size_Net_1)
{
    std::cerr << "Starting IPv6_Set_Size_Net_1 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1, 16);

    CHECK_EQUAL(204u, s1.memory_size());
}

TEST(IPv6_Set_Size_2)
{
    std::cerr << "Starting IPv6_Set_Size_2 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    s1.add(a1);
    s1.add(a2);

    CHECK_EQUAL(2844u, s1.memory_size());
}

TEST(IPv6_Set_Size_Net_2)
{
    std::cerr << "Starting IPv6_Set_Size_Net_2 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    s1.add(a1, 16);
    s1.add(a2, 16);

    CHECK_EQUAL(300u, s1.memory_size());
}

TEST(IPv6_Set_Contains_1)
{
    std::cerr << "Starting IPv6_Set_Contains_1 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1);

    CHECK(s1.contains(a1));
}

TEST(IPv6_Set_Contains_Net_1)
{
    std::cerr << "Starting IPv6_Set_Contains_Net_1 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1, 16);

    CHECK(s1.contains(a1));
}

TEST(IPv6_Set_Contains_2)
{
    std::cerr << "Starting IPv6_Set_Contains_2 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    s1.add(a1);
    s1.add(a2);

    CHECK(s1.contains(a1));
    CHECK(s1.contains(a2));
}

TEST(IPv6_Set_Contains_Net_2)
{
    std::cerr << "Starting IPv6_Set_Contains_Net_2 test case." << std::endl;

    set_t  s1;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    s1.add(a1, 16);
    s1.add(a2, 16);

    CHECK(s1.contains(a1));
    CHECK(s1.contains(a2));
}

TEST(IPv6_Set_Equal_1)
{
    std::cerr << "Starting IPv6_Set_Equal_1 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1);
    s2.add(a1);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv6_Set_Equal_Net_1)
{
    std::cerr << "Starting IPv6_Set_Equal_Net_1 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    s1.add(a1, 16);
    s2.add(a1, 16);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv6_Set_Equal_2)
{
    std::cerr << "Starting IPv6_Set_Equal_2 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    s1.add(a1);
    s1.add(a2);

    s2.add(a1);
    s2.add(a2);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv6_Set_Equal_Net_2)
{
    std::cerr << "Starting IPv6_Set_Equal_Net_2 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    s1.add(a1, 16);
    s1.add(a2, 16);

    s2.add(a1, 16);
    s2.add(a2, 16);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv6_Gen_Set_Equal_1)
{
    std::cerr << "Starting IPv6_Gen_Set_Equal_1 test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ga1(a1);

    s1.add(a1);
    s2.add(ga1);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv6_Set_Idempotent)
{
    std::cerr << "Starting IPv6_Set_Idempotent test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    CHECK(!s1.add(a1));
    CHECK(s1.add(a1));

    s2.add(a1);

    CHECK_EQUAL(s1, s2);
}

TEST(IPv6_Set_Idempotent_Net)
{
    std::cerr << "Starting IPv6_Set_Idempotent_Net test case." << std::endl;

    set_t  s1;
    set_t  s2;
    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);

    CHECK(!s1.add(a1, 16));
    CHECK(s1.add(a1, 16));

    s2.add(a1, 16);

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
