/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <glib.h>

#include <ipset/bdd/nodes.h>
#include <ipset/logging.h>


ipset_assignment_t *
ipset_assignment_new()
{
    ipset_assignment_t  *assignment;

    assignment = g_slice_new(ipset_assignment_t);
    assignment->values =
        g_array_new(FALSE, FALSE, sizeof(ipset_tribool_t));

    return assignment;
}


void
ipset_assignment_free(ipset_assignment_t *assignment)
{
    g_array_free(assignment->values, TRUE);
    g_slice_free(ipset_assignment_t, assignment);
}


gboolean
ipset_assignment_equal(const ipset_assignment_t *assignment1,
                       const ipset_assignment_t *assignment2)
{
    /*
     * Identical pointers are trivially equal.
     */

    if (assignment1 == assignment2)
    {
        return TRUE;
    }

    /*
     * Otherwise we compare the assignments piecewise up through the
     * end of the smaller vector.
     */

    guint  size1 = assignment1->values->len;
    guint  size2 = assignment2->values->len;
    guint  smaller_size = (size1 < size2)? size1: size2;

    guint  i;
    for (i = 0; i < smaller_size; i++)
    {
        if (g_array_index(assignment1->values, ipset_tribool_t, i) !=
            g_array_index(assignment2->values, ipset_tribool_t, i))
        {
            return FALSE;
        }
    }

    /*
     * If one of the assignment vectors is longer, any remaining
     * elements must be indeterminate.
     */

    if (size1 > smaller_size)
    {
        for (i = smaller_size; i < size1; i++)
        {
            if (g_array_index(assignment1->values,
                              ipset_tribool_t, i) != IPSET_EITHER)
            {
                return FALSE;
            }
        }
    }

    if (size2 > smaller_size)
    {
        for (i = smaller_size; i < size2; i++)
        {
            if (g_array_index(assignment2->values,
                              ipset_tribool_t, i) != IPSET_EITHER)
            {
                return FALSE;
            }
        }
    }

    /*
     * If we make it through all of that, the two assignments are
     * equal.
     */

    return TRUE;
}


void
ipset_assignment_cut(ipset_assignment_t *assignment,
                     ipset_variable_t var)
{
    if (var < assignment->values->len)
    {
        g_array_set_size(assignment->values, var);
    }
}


void
ipset_assignment_clear(ipset_assignment_t *assignment)
{
    ipset_assignment_cut(assignment, 0);
}


ipset_tribool_t
ipset_assignment_get(ipset_assignment_t *assignment,
                     ipset_variable_t var)
{
    if (var < assignment->values->len)
    {
        /*
         * If the requested variable is in the range of the values
         * array, return whatever is stored there.
         */

        return g_array_index
            (assignment->values, ipset_tribool_t, var);

    } else {
        /*
         * Variables htat aren't in the values array are always
         * EITHER.
         */

        return IPSET_EITHER;
    }
}


void
ipset_assignment_set(ipset_assignment_t *assignment,
                     ipset_variable_t var,
                     ipset_tribool_t value)
{
    /*
     * Ensure that the vector is big enough to hold this variable
     * assignment, inserting new EITHERs if needed.
     */

    if (var >= assignment->values->len)
    {
        guint  old_len = assignment->values->len;

        /*
         * Expand the array.
         */

        g_array_set_size(assignment->values, var + 1);

        /*
         * Fill in EITHERs in the newly allocated elements.
         */

        if (var != old_len)
        {
            guint  i;
            for (i = old_len; i < var; i++)
            {
                g_array_index(assignment->values, ipset_tribool_t, i) =
                    IPSET_EITHER;
            }
        }
    }

    /*
     * Assign the desired value.
     */

    g_array_index(assignment->values, ipset_tribool_t, var) = value;
}
