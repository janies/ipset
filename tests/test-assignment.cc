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

#include <boost/dynamic_bitset.hpp>
#include <boost/logic/tribool.hpp>

// Both of the following try to define CHECK.  We want the one from
// UnitTest.
#include <glog/logging.h>
#undef CHECK
#include <UnitTest++.h>

#include <ip/bdd/nodes.hh>

using namespace boost::logic;
using namespace ip::bdd;


//--------------------------------------------------------------------
// Assignments

TEST(BDD_Assignment_Empty_Equal)
{
    std::cerr << "Starting BDD_Assignment_Empty_Equal test case." << std::endl;

    assignment_t  a1;
    assignment_t  a2;

    CHECK_EQUAL(a1, a2);
}

TEST(BDD_Assignment_Equal_1)
{
    std::cerr << "Starting BDD_Assignment_Equal_1 test case." << std::endl;

    assignment_t  a1;
    assignment_t  a2;

    a1[0] = true;
    a1[1] = false;

    a2[0] = true;
    a2[1] = false;

    CHECK_EQUAL(a1, a2);
}

TEST(BDD_Assignment_Equal_2)
{
    std::cerr << "Starting BDD_Assignment_Equal_2 test case." << std::endl;

    assignment_t  a1;
    assignment_t  a2;

    a1[0] = true;
    a1[1] = false;

    a2[0] = true;
    a2[1] = false;
    a2[4] = indeterminate;

    CHECK_EQUAL(a1, a2);
}

TEST(BDD_Assignment_Cut_1)
{
    std::cerr << "Starting BDD_Assignment_Cut_1 test case." << std::endl;

    assignment_t  a1;
    assignment_t  a2;

    a1[0] = true;
    a1[1] = false;

    a2[0] = true;
    a2[1] = false;
    a2[2] = true;
    a2[3] = true;
    a2[4] = false;

    a2.cut(2);

    CHECK_EQUAL(a1, a2);
}


//--------------------------------------------------------------------
// Expanded assignments

TEST(BDD_Assignment_Expand_1)
{
    std::cerr << "Starting BDD_Assignment_Expand_1 test case." << std::endl;

    assignment_t  a;

    a[0] = true;
    a[1] = false;

    assignment_t::iterator  it = a.begin(2);
    boost::dynamic_bitset<>  ea(2);

    ea[0] = true;
    ea[1] = false;
    CHECK_EQUAL(ea, *it);

    ++it;
    CHECK(it == a.end());
}

TEST(BDD_Assignment_Expand_2)
{
    std::cerr << "Starting BDD_Assignment_Expand_2 test case." << std::endl;

    assignment_t  a;

    a[0] = true;
    a[1] = false;

    assignment_t::iterator  it = a.begin(3);
    boost::dynamic_bitset<>  ea(3);

    ea[0] = true;
    ea[1] = false;
    ea[2] = false;
    CHECK_EQUAL(ea, *it);

    ++it;
    ea[0] = true;
    ea[1] = false;
    ea[2] = true;
    CHECK_EQUAL(ea, *it);

    ++it;
    CHECK(it == a.end());
}

TEST(BDD_Assignment_Expand_3)
{
    std::cerr << "Starting BDD_Assignment_Expand_3 test case." << std::endl;

    assignment_t  a;

    a[0] = true;
    a[2] = false;

    assignment_t::iterator  it = a.begin(3);
    boost::dynamic_bitset<>  ea(3);

    ea[0] = true;
    ea[1] = false;
    ea[2] = false;
    CHECK_EQUAL(ea, *it);

    ++it;
    ea[0] = true;
    ea[1] = true;
    ea[2] = false;
    CHECK_EQUAL(ea, *it);

    ++it;
    CHECK(it == a.end());
}


//--------------------------------------------------------------------
// Boilerplate

int main(int argc, char **argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
    return UnitTest::RunAllTests();
}
