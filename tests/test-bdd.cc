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

// Both of the following try to define CHECK.  We want the one from
// UnitTest.
#include <glog/logging.h>
#undef CHECK
#include <UnitTest++.h>

#include <ip/bdd/engine.hh>
#include <ip/bdd/nodes.hh>
#include <ip/bdd/set.hh>

using namespace ip::bdd;


//--------------------------------------------------------------------
// BDD terminals

TEST(BDD_False_Terminal)
{
    std::cerr << "Starting BDD_False_Terminal test case." << std::endl;

    engine_t<bool>  engine;

    types<bool>::node  n_false = engine.terminal(false);
    types<bool>::terminal  t_false = n_false.as_terminal();

    CHECK_EQUAL(false, t_false->value());
}


TEST(BDD_True_Terminal)
{
    std::cerr << "Starting BDD_True_Terminal test case." << std::endl;

    engine_t<bool>  engine;

    types<bool>::node  n_true = engine.terminal(true);
    types<bool>::terminal  t_true = n_true.as_terminal();

    CHECK_EQUAL(true, t_true->value());
}


TEST(BDD_Terminal_NotReduced_1)
{
    std::cerr << "Starting BDD_Terminal_NotReduced_1 test case." << std::endl;

    // If we create terminals by hand, they won't be reduced — i.e.,
    // we can have two terminals with the same value but at different
    // memory locations.

    types<bool>::terminal  node1(new terminal_t<bool>(false));
    types<bool>::terminal  node2(new terminal_t<bool>(false));

    CHECK(node1 != node2);
}


TEST(BDD_Terminal_Reduced_1)
{
    std::cerr << "Starting BDD_Terminal_Reduced_1 test case." << std::endl;

    // If we create terminals via a BDD engine, they will be reduced —
    // i.e., every terminal with the same value will be in the same
    // memory location.

    engine_t<bool>  engine;

    types<bool>::node  node1 = engine.terminal(false);
    types<bool>::node  node2 = engine.terminal(false);

    CHECK_EQUAL(node1, node2);
}


//--------------------------------------------------------------------
// BDD non-terminals

TEST(BDD_Nonterminal_1)
{
    std::cerr << "Starting BDD_Nonterminal_1 test case." << std::endl;

    engine_t<bool>  engine;

    types<bool>::node  n_false = engine.terminal(false);
    types<bool>::node  n_true = engine.terminal(true);

    types<bool>::node  node =
        engine.nonterminal(0, n_false, n_true);
    types<bool>::nonterminal  nonterminal =
        node.as_nonterminal();

    CHECK_EQUAL(0u, nonterminal->variable());
    CHECK_EQUAL(n_false, nonterminal->low());
    CHECK_EQUAL(n_true, nonterminal->high());
}


TEST(BDD_Nonterminal_NotReduced_1)
{
    std::cerr << "Starting BDD_Nonterminal_NotReduced_1 test case." << std::endl;

    // If we create nonterminals by hand, they won't be reduced —
    // i.e., we can have two nonterminals with the same value but at
    // different memory locations.

    types<bool>::terminal  t_false(new terminal_t<bool>(false));
    types<bool>::terminal  t_true(new terminal_t<bool>(true));

    types<bool>::nonterminal  node1
        (new nonterminal_t<bool>(0, t_false, t_true));
    types<bool>::nonterminal  node2
        (new nonterminal_t<bool>(0, t_false, t_true));

    CHECK(node1 != node2);
}


TEST(BDD_Nonterminal_Reduced_1)
{
    std::cerr << "Starting BDD_Nonterminal_Reduced_1 test case." << std::endl;

    // If we create nonterminals via a BDD engine, they will be
    // reduced — i.e., every nonterminal with the same value will be
    // in the same memory location.

    engine_t<bool>  engine;

    types<bool>::node  n_false = engine.terminal(false);
    types<bool>::node  n_true = engine.terminal(true);

    types<bool>::node  node1 =
        engine.nonterminal(0, n_false, n_true);
    types<bool>::node  node2 =
        engine.nonterminal(0, n_false, n_true);

    CHECK_EQUAL(node1, node2);
}


TEST(BDD_Nonterminal_Reduced_2)
{
    std::cerr << "Starting BDD_Nonterminal_Reduced_2 test case." << std::endl;

    // We shouldn't have a nonterminal whose low and high subtrees are
    // equal.

    engine_t<bool>  engine;

    types<bool>::node  n_false = engine.terminal(false);

    types<bool>::node  node =
        engine.nonterminal(0, n_false, n_false);

    CHECK_EQUAL(node, n_false);
}


//--------------------------------------------------------------------
// Evaluation

TEST(BDD_Evaluate_1)
{
    std::cerr << "Starting BDD_Evaluate_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = ¬x[0]

    engine_t<bool>  engine;

    types<bool>::node  n_false = engine.terminal(false);
    types<bool>::node  n_true = engine.terminal(true);

    types<bool>::node  node = engine.nonterminal(0, n_true, n_false);

    // And test we can get the right results out of it.

    bool  input1[] = { true };
    bool  input2[] = { false };

    CHECK_EQUAL(false, node.evaluate(input1));
    CHECK_EQUAL(true, node.evaluate(input2));
}


TEST(BDD_Evaluate_2)
{
    std::cerr << "Starting BDD_Evaluate_2 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = ¬x[0] ∧ x[1]

    engine_t<bool>  engine;

    types<bool>::node  n_false = engine.terminal(false);
    types<bool>::node  n_true = engine.terminal(true);

    types<bool>::node  node1 = engine.nonterminal(1, n_false, n_true);
    types<bool>::node  node = engine.nonterminal(0, node1, n_false);

    // And test we can get the right results out of it.

    bool  input1[] = { true, true };
    bool  input2[] = { true, false };
    bool  input3[] = { false, true };
    bool  input4[] = { false, false };

    CHECK_EQUAL(false, node.evaluate(input1));
    CHECK_EQUAL(false, node.evaluate(input2));
    CHECK_EQUAL(true, node.evaluate(input3));
    CHECK_EQUAL(false, node.evaluate(input4));
}


//--------------------------------------------------------------------
// Operators

TEST(BDD_And_Reduced_1)
{
    std::cerr << "Starting BDD_And_Reduced_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = x[0] ∧ x[1]

    bool_engine_t  engine;

    types<bool>::node  n_false0 = engine.terminal(false);
    types<bool>::node  n_true0 = engine.terminal(true);

    types<bool>::node  node00 = engine.nonterminal(0, n_false0, n_true0);
    types<bool>::node  node01 = engine.nonterminal(1, n_false0, n_true0);
    types<bool>::node  node0 = engine.apply_and(node00, node01);

    // And then do it again.

    types<bool>::node  n_false1 = engine.terminal(false);
    types<bool>::node  n_true1 = engine.terminal(true);

    types<bool>::node  node10 = engine.nonterminal(0, n_false1, n_true1);
    types<bool>::node  node11 = engine.nonterminal(1, n_false1, n_true1);
    types<bool>::node  node1 = engine.apply_and(node10, node11);

    // Verify that we get the same physical node both times.

    CHECK_EQUAL(node0, node1);
}


TEST(BDD_And_Evaluate_1)
{
    std::cerr << "Starting BDD_And_1 test case." << std::endl;

    // Create a BDD representing
    //   f(x) = x[0] ∧ x[1]

    bool_engine_t  engine;

    types<bool>::node  n_false = engine.terminal(false);
    types<bool>::node  n_true = engine.terminal(true);

    types<bool>::node  node0 = engine.nonterminal(0, n_false, n_true);
    types<bool>::node  node1 = engine.nonterminal(1, n_false, n_true);
    types<bool>::node  node = engine.apply_and(node0, node1);

    // And test we can get the right results out of it.

    bool  input1[] = { true, true };
    bool  input2[] = { true, false };
    bool  input3[] = { false, true };
    bool  input4[] = { false, false };

    CHECK_EQUAL(true, node.evaluate(input1));
    CHECK_EQUAL(false, node.evaluate(input2));
    CHECK_EQUAL(false, node.evaluate(input3));
    CHECK_EQUAL(false, node.evaluate(input4));
}


//--------------------------------------------------------------------
// Boilerplate

int main(int argc, char **argv)
{
    FLAGS_logtostderr = true;
    google::InitGoogleLogging(argv[0]);
    return UnitTest::RunAllTests();
}
