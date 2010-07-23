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


static void
initialize(ipset_expanded_assignment_t *exp,
           const ipset_assignment_t *assignment,
           ipset_variable_t last_var)
{
    /*
     * First loop through all of the variables in the assignment
     * vector, making sure not to go further than the caller
     * requested.
     */

    ipset_variable_t  last_assignment = assignment->values->len;
    if (last_var < last_assignment)
        last_assignment = last_var;

    ipset_variable_t  var;
    for (var = 0; var < last_assignment; var++)
    {
        ipset_tribool_t  curr_value =
            g_array_index(assignment->values, ipset_tribool_t, var);

        if (curr_value == IPSET_EITHER)
        {
            /*
             * If this variable is EITHER, start it off as FALSE, and
             * add it to the eithers list.
             */

            g_d_debug("Variable %u is EITHER", var);

            IPSET_BIT_SET(exp->values->data, var, FALSE);
            g_array_append_val(exp->eithers, var);
        } else {
            /*
             * Otherwise set the variable to the same value in the
             * expanded assignment as it is in the non-expanded one.
             */

            g_d_debug("Variable %u is %s", var,
                      curr_value? "TRUE": "FALSE");

            IPSET_BIT_SET(exp->values->data, var, curr_value);
        }
    }

    /*
     * If the caller requested more variables than there are in the
     * assignment vector, add them to the eithers list.
     */

    for (var = last_assignment; var < last_var; var++)
    {
        g_d_debug("Variable %u is implicitly EITHER", var);
        g_array_append_val(exp->eithers, var);
    }
}


ipset_expanded_assignment_t *
ipset_assignment_expand(const ipset_assignment_t *assignment,
                        ipset_variable_t last_var)
{
    /*
     * First allocate the iterator itself, and all of its contained
     * fields.
     */

    ipset_expanded_assignment_t  *exp;

    guint  values_size =
        (last_var / 8) + ((last_var % 8) != 0);

    exp = g_slice_new(ipset_expanded_assignment_t);
    exp->finished = FALSE;
    exp->values = g_byte_array_sized_new(values_size);
    exp->eithers =
        g_array_new(FALSE, FALSE, sizeof(ipset_variable_t));

    /*
     * Then initialize the values and eithers fields.
     */

    initialize(exp, assignment, last_var);

    return exp;
}


void
ipset_expanded_assignment_free(ipset_expanded_assignment_t *exp)
{
    g_byte_array_free(exp->values, TRUE);
    g_array_free(exp->eithers, TRUE);
    g_slice_free(ipset_expanded_assignment_t, exp);
}


void
ipset_expanded_assignment_advance(ipset_expanded_assignment_t *exp)
{
    /*
     * If we're already at the end of the iterator, don't do anything.
     */

    if (G_UNLIKELY(exp->finished))
        return;

    g_d_debug("Advancing iterator");

    /*
     * Look at the last EITHER bit in the assignment.  If it's 0, then
     * set it to 1 and return.  Otherwise we set it to 0 and carry up
     * to the previous indeterminate bit.
     */

    guint  i;
    for (i = exp->eithers->len; i > 0; i--)
    {
        guint  idx = i - 1;
        ipset_variable_t  either_var =
            g_array_index(exp->eithers, ipset_variable_t, idx);

        g_d_debug("Checking EITHER variable %u", either_var);

        if (IPSET_BIT_GET(exp->values->data, either_var))
        {
            /*
             * This variable is currently TRUE, so set it back to
             * FALSE and carry.
             */

            g_d_debug("  Variable %u is TRUE, changing to FALSE"
                      " and carrying",
                      either_var);
            IPSET_BIT_SET(exp->values->data, either_var, FALSE);
        } else {
            /*
             * This variable is currently FALSE, so set it to TRUE and
             * return.
             */

            g_d_debug("  Variable %u is FALSE, changing to TRUE",
                      either_var);
            IPSET_BIT_SET(exp->values->data, either_var, TRUE);
            return;
        }
    }

    /*
     * If we fall through then we've made it through all of the
     * expanded assignments.
     */

    exp->finished = TRUE;
}
