#ifndef UNITTEST_CHECKS_H
#define UNITTEST_CHECKS_H

#include <iomanip>
#include <ostream>
#include <string>

#include "Config.h"
#include "TestResults.h"
#include "MemoryOutStream.h"

namespace UnitTest {


/**
 * A wrapper class around a std::string that outputs as a hexdump.
 * Useful for comparing binary data in unit tests.
 */

struct binary_string
{
    const std::string  &str;

    binary_string(const std::string &str_): str(str_) { }

    bool
    operator == (const binary_string &other) const
    {
        return (str == other.str);
    }

    bool
    operator == (const std::string &other) const
    {
        return (str == other);
    }
};

std::ostream &
operator << (std::ostream &stream, const binary_string &bin)
{
    bool comma_needed = false;

    stream << std::dec << bin.str.length() << ":[";

    for (unsigned int i = 0; i < bin.str.length(); i++)
    {
        if (comma_needed)
            stream << ',';
        else
            comma_needed = true;

        stream << std::hex
               << std::setfill('0')
               << std::setw(2)
               << (int) (unsigned char) bin.str[i];
    }

    stream << ']';
    return stream;
}


template< typename Value >
bool Check(Value const value)
{
    return !!value; // doing double negative to avoid silly VS warnings
}


template< typename Expected, typename Actual >
void CheckEqual(TestResults& results, Expected const& expected, Actual const& actual, TestDetails const& details)
{
    if (!(expected == actual))
    {
        UnitTest::MemoryOutStream stream;
        stream << "Expected " << expected << " but was " << actual;

        results.OnTestFailure(details, stream.GetText());
    }
}

void CheckEqual(TestResults& results, char const* expected, char const* actual, TestDetails const& details);

void CheckEqual(TestResults& results, char* expected, char* actual, TestDetails const& details);

void CheckEqual(TestResults& results, char* expected, char const* actual, TestDetails const& details);

void CheckEqual(TestResults& results, char const* expected, char* actual, TestDetails const& details);

template< typename Expected, typename Actual, typename Tolerance >
bool AreClose(Expected const& expected, Actual const& actual, Tolerance const& tolerance)
{
    return (actual >= (expected - tolerance)) && (actual <= (expected + tolerance));
}

template< typename Expected, typename Actual, typename Tolerance >
void CheckClose(TestResults& results, Expected const& expected, Actual const& actual, Tolerance const& tolerance,
                TestDetails const& details)
{
    if (!AreClose(expected, actual, tolerance))
    { 
        UnitTest::MemoryOutStream stream;
        stream << "Expected " << expected << " +/- " << tolerance << " but was " << actual;

        results.OnTestFailure(details, stream.GetText());
    }
}


template< typename Expected, typename Actual >
void CheckArrayEqual(TestResults& results, Expected const& expected, Actual const& actual,
                int const count, TestDetails const& details)
{
    bool equal = true;
    for (int i = 0; i < count; ++i)
        equal &= (expected[i] == actual[i]);

    if (!equal)
    {
        UnitTest::MemoryOutStream stream;

		stream << "Expected [ ";

		for (int expectedIndex = 0; expectedIndex < count; ++expectedIndex)
            stream << expected[expectedIndex] << " ";

		stream << "] but was [ ";

		for (int actualIndex = 0; actualIndex < count; ++actualIndex)
            stream << actual[actualIndex] << " ";

		stream << "]";

        results.OnTestFailure(details, stream.GetText());
    }
}

template< typename Expected, typename Actual, typename Tolerance >
bool ArrayAreClose(Expected const& expected, Actual const& actual, int const count, Tolerance const& tolerance)
{
    bool equal = true;
    for (int i = 0; i < count; ++i)
        equal &= AreClose(expected[i], actual[i], tolerance);
    return equal;
}

template< typename Expected, typename Actual, typename Tolerance >
void CheckArrayClose(TestResults& results, Expected const& expected, Actual const& actual,
                   int const count, Tolerance const& tolerance, TestDetails const& details)
{
    bool equal = ArrayAreClose(expected, actual, count, tolerance);

    if (!equal)
    {
        UnitTest::MemoryOutStream stream;

        stream << "Expected [ ";
        for (int expectedIndex = 0; expectedIndex < count; ++expectedIndex)
            stream << expected[expectedIndex] << " ";
        stream << "] +/- " << tolerance << " but was [ ";

		for (int actualIndex = 0; actualIndex < count; ++actualIndex)
            stream << actual[actualIndex] << " ";
        stream << "]";

        results.OnTestFailure(details, stream.GetText());
    }
}

template< typename Expected, typename Actual, typename Tolerance >
void CheckArray2DClose(TestResults& results, Expected const& expected, Actual const& actual,
                   int const rows, int const columns, Tolerance const& tolerance, TestDetails const& details)
{
    bool equal = true;
    for (int i = 0; i < rows; ++i)
        equal &= ArrayAreClose(expected[i], actual[i], columns, tolerance);

    if (!equal)
    {
        UnitTest::MemoryOutStream stream;

        stream << "Expected [ ";    

		for (int expectedRow = 0; expectedRow < rows; ++expectedRow)
        {
            stream << "[ ";
            for (int expectedColumn = 0; expectedColumn < columns; ++expectedColumn)
                stream << expected[expectedRow][expectedColumn] << " ";
            stream << "] ";
        }

		stream << "] +/- " << tolerance << " but was [ ";

		for (int actualRow = 0; actualRow < rows; ++actualRow)
        {
            stream << "[ ";
            for (int actualColumn = 0; actualColumn < columns; ++actualColumn)
                stream << actual[actualRow][actualColumn] << " ";
            stream << "] ";
        }

		stream << "]";

        results.OnTestFailure(details, stream.GetText());
    }
}

}

#endif
