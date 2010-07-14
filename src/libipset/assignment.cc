/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <vector>

#include <boost/logic/tribool.hpp>
#include <glog/logging.h>

#include <ip/bdd/nodes.hh>

namespace ip {
namespace bdd {


bool
assignment_t::operator == (const assignment_t &other) const
{
    // Identical objects are trivially equal.

    if (&other == this)
        return true;

    // Otherwise we compare the assignments piecewise up through
    // the end of the smaller vector.

    value_list_t::size_type  this_size = values.size();
    value_list_t::size_type  other_size = other.values.size();
    value_list_t::size_type  smaller_size =
        (this_size < other_size)? this_size: other_size;

    for (value_list_t::size_type i = 0; i < smaller_size; i++)
    {
        if (values[i].value != other.values[i].value)
            return false;
    }

    // If one of the assignment vectors is longer, any remaining
    // elements must be indeterminate.

    if (this_size > smaller_size)
    {
        for (value_list_t::size_type i = smaller_size;
             i < this_size; i++)
        {
            if (!boost::logic::indeterminate(values[i]))
                return false;
        }
    } else if (other_size > smaller_size)
    {
        for (value_list_t::size_type i = smaller_size;
             i < other_size; i++)
        {
            if (!boost::logic::indeterminate(other.values[i]))
                return false;
        }
    }

    // They're equal

    return true;
}


bool
assignment_t::operator != (const assignment_t &other) const
{
    return !(this->operator == (other));
}


std::ostream &
operator << (std::ostream &stream, const assignment_t &assignment)
{
    bool need_comma = false;

    stream << '[';

    for (assignment_t::value_list_t::size_type  i = 0;
         i < assignment.values.size(); i++)
    {
        // Only print out the determinate assignments.

        if (!boost::logic::indeterminate(assignment.values[i]))
        {
            if (need_comma)
                stream << ", ";
            else
                need_comma = true;

            if (!assignment.values[i])
            {
                stream << '!';
            }

            stream << 'x' << i;
        }
    }

    stream << ']';

    return stream;
}


} // namespace bdd
} // namespace ip
