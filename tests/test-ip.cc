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

#include <ip/endian.hh>
#include <ip/ip.hh>

using namespace ip;


//--------------------------------------------------------------------
// IPv4

TEST(IPv4_Equal_1)
{
    std::cerr << "Starting IPv4_Equal_1 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(ip::endian::host_to_big(0xc0a80001));

    CHECK_EQUAL(a1, a2);
}


TEST(IPv4_Equal_2)
{
    std::cerr << "Starting IPv4_Equal_2 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(192,168,0,2);

    CHECK(a1 != a2);
}


TEST(IPv4_Equal_3)
{
    std::cerr << "Starting IPv4_Equal_3 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);

    CHECK(a1 != a2);
}


TEST(IPv4_Raw)
{
    std::cerr << "Starting IPv4_Raw test case." << std::endl;

    ipv4_addr_t  a(192,168,0,1);
    uint8_t  expected[] = "\xc0\xa8\x00\x01";

    CHECK(memcmp(expected, (const uint8_t *) a, 4) == 0);
}


TEST(IPv4_String)
{
    std::cerr << "Starting IPv4_String test case." << std::endl;

    ipv4_addr_t  a(192,168,0,1);

    std::ostringstream  s;
    s << a;

    CHECK_EQUAL("192.168.0.1", s.str());
}


TEST(IPv4_Dec_String)
{
    std::cerr << "Starting IPv4_Dec_String test case." << std::endl;

    ipv4_addr_t  a(192,168,0,1);

    std::ostringstream  s;
    s << a.host_endian();

    CHECK_EQUAL("3232235521", s.str());
}


TEST(IPv4_Copy_1)
{
    std::cerr << "Starting IPv4_Copy_1 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(a1);

    CHECK_EQUAL(a1, a2);
}


//--------------------------------------------------------------------
// IPv6

TEST(IPv6_Equal_1)
{
    std::cerr << "Starting IPv6_Equal_1 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);

    CHECK_EQUAL(a1, a2);
}


TEST(IPv6_Equal_2)
{
    std::cerr << "Starting IPv6_Equal_2 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    CHECK(a1 != a2);
}


TEST(IPv6_Equal_3)
{
    std::cerr << "Starting IPv6_Equal_3 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv4_addr_t  a2(192,168,0,1);

    CHECK(a1 != a2);
}


TEST(IPv6_Raw)
{
    std::cerr << "Starting IPv6_Raw test case." << std::endl;

    ipv6_addr_t  a(0xfe80,0,0,0,0,0,0,1);
    uint8_t  expected[] =
        "\xfe\x80\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x01";

    CHECK(memcmp(expected, (const uint8_t *) a, 16) == 0);
}


TEST(IPv6_String_1)
{
    std::cerr << "Starting IPv6_String_1 test case." << std::endl;

    ipv6_addr_t  a(0xfe80,0,0,0,0,0,0,1);

    std::ostringstream  s;
    s << a;

    CHECK_EQUAL("fe80::1", s.str());
}


TEST(IPv6_String_2)
{
    std::cerr << "Starting IPv6_String_2 test case." << std::endl;

    ipv6_addr_t  a(0,0,0,0,0,0xffff,0xc0a8,0x0001);

    std::ostringstream  s;
    s << a;

    CHECK_EQUAL("::ffff:192.168.0.1", s.str());
}


TEST(IPv6_Copy_1)
{
    std::cerr << "Starting IPv6_Copy_1 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(a1);

    CHECK_EQUAL(a1, a2);
}


//--------------------------------------------------------------------
// Generic IP

TEST(Gen_IPv4_Version)
{
    std::cerr << "Starting Gen_IPv4_Version test case." << std::endl;

    ipv4_addr_t  a(192,168,0,1);
    ip_addr_t  ip(a);

    CHECK_EQUAL(4, ip.version());
}


TEST(Gen_IPv4_Equal_1)
{
    std::cerr << "Starting Gen_IPv4_Equal_1 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ip_addr_t  ip1(a1);
    ipv4_addr_t  a2(ip::endian::host_to_big(0xc0a80001));
    ip_addr_t  ip2(a2);

    CHECK_EQUAL(ip1, ip2);
}


TEST(Gen_IPv4_Equal_2)
{
    std::cerr << "Starting Gen_IPv4_Equal_2 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ip_addr_t  ip1(a1);
    ipv4_addr_t  a2(192,168,0,2);
    ip_addr_t  ip2(a2);

    CHECK(ip1 != ip2);
}


TEST(Gen_IPv4_Equal_3)
{
    std::cerr << "Starting Gen_IPv4_Equal_3 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ip_addr_t  ip1(a1);
    ipv4_addr_t  a2(192,168,0,1);

    CHECK_EQUAL(ip1, a2);
}


TEST(Gen_IPv4_Equal_4)
{
    std::cerr << "Starting Gen_IPv4_Equal_4 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ipv4_addr_t  a2(192,168,0,1);
    ip_addr_t  ip2(a2);

    CHECK_EQUAL(a1, ip2);
}


TEST(Gen_IPv4_Raw)
{
    std::cerr << "Starting Gen_IPv4_Raw test case." << std::endl;

    ipv4_addr_t  a(192,168,0,1);
    ip_addr_t  ip(a);
    uint8_t  expected[] = "\xc0\xa8\x00\x01";

    CHECK(memcmp(expected, (const uint8_t *) ip, 4) == 0);
}


TEST(Gen_IPv4_String)
{
    std::cerr << "Starting Gen_IPv4_String test case." << std::endl;

    ipv4_addr_t  a(192,168,0,1);
    ip_addr_t  ip(a);

    std::ostringstream  s;
    s << ip;

    CHECK_EQUAL("192.168.0.1", s.str());
}


TEST(Gen_IPv4_Copy_1)
{
    std::cerr << "Starting Gen_IPv4_Copy_1 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ip_addr_t  ip1(a1);
    ip_addr_t  ip2(ip1);

    CHECK_EQUAL(ip1, ip2);
}


TEST(Gen_IPv6_Version)
{
    std::cerr << "Starting Gen_IPv6_Version test case." << std::endl;

    ipv6_addr_t  a(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip(a);

    CHECK_EQUAL(6, ip.version());
}


TEST(Gen_IPv6_Equal_1)
{
    std::cerr << "Starting Gen_IPv6_Equal_1 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip1(a1);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip2(a2);

    CHECK_EQUAL(ip1, ip2);
}


TEST(Gen_IPv6_Equal_2)
{
    std::cerr << "Starting Gen_IPv6_Equal_2 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip1(a1);
    ipv6_addr_t  a2(0,0,0,0,0,0xffff,0xc0a8,0x0001);
    ip_addr_t  ip2(a2);

    CHECK(ip1 != ip2);
}


TEST(Gen_IPv6_Equal_3)
{
    std::cerr << "Starting Gen_IPv6_Equal_3 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip1(a1);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);

    CHECK_EQUAL(ip1, a2);
}


TEST(Gen_IPv6_Equal_4)
{
    std::cerr << "Starting Gen_IPv6_Equal_4 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip2(a2);

    CHECK_EQUAL(a1, ip2);
}


TEST(Gen_IPv6_Raw)
{
    std::cerr << "Starting Gen_IPv6_Raw test case." << std::endl;

    ipv6_addr_t  a(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip(a);
    uint8_t  expected[] =
        "\xfe\x80\x00\x00\x00\x00\x00\x00"
        "\x00\x00\x00\x00\x00\x00\x00\x01";

    CHECK(memcmp(expected, (const uint8_t *) ip, 16) == 0);
}


TEST(Gen_IPv6_String_1)
{
    std::cerr << "Starting Gen_IPv6_String_1 test case." << std::endl;

    ipv6_addr_t  a(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip(a);

    std::ostringstream  s;
    s << ip;

    CHECK_EQUAL("fe80::1", s.str());
}


TEST(Gen_IPv6_String_2)
{
    std::cerr << "Starting Gen_IPv6_String_2 test case." << std::endl;

    ipv6_addr_t  a(0,0,0,0,0,0xffff,0xc0a8,0x0001);
    ip_addr_t  ip(a);

    std::ostringstream  s;
    s << ip;

    CHECK_EQUAL("::ffff:192.168.0.1", s.str());
}


TEST(Gen_IPv6_Copy_1)
{
    std::cerr << "Starting Gen_IPv6_Copy_1 test case." << std::endl;

    ipv6_addr_t  a1(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip1(a1);
    ip_addr_t  ip2(ip1);

    CHECK_EQUAL(ip1, ip2);
}


TEST(Gen_IP_Equal_1)
{
    std::cerr << "Starting Gen_IP_Equal_1 test case." << std::endl;

    ipv4_addr_t  a1(192,168,0,1);
    ip_addr_t  ip1(a1);
    ipv6_addr_t  a2(0xfe80,0,0,0,0,0,0,1);
    ip_addr_t  ip2(a2);

    CHECK(ip1 != ip2);
}


//--------------------------------------------------------------------
// Boilerplate

int main(int argc, char **argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
    return UnitTest::RunAllTests();
}
