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
#include <ipset/ipset.h>
#include <ipset/logging.h>


/* Forward declarations */

static void
process_assignment(ipset_iterator_t *iterator);


/**
 * Create a generic IP address object from the current expanded
 * assignment.
 */

static void
create_ip_address(ipset_ip_t *addr, ipset_expanded_assignment_t *exp)
{
    /*
     * Check variable 0 to see if this is an IPv4 or IPv6 address.
     */

    if (IPSET_BIT_GET(exp->values->data, 0))
    {
        /*
         * TRUE means IPv4
         */

        addr->is_ipv4 = TRUE;

        /*
         * Copying the bits into the IP address is a pain since
         * everything is off by one bit.  Joy.
         */

        guint8  *src = exp->values->data;
        guint8  *dest = (guint8 *) addr->addr;

        gint  i;

        for (i = 0; i < 4; i++)
        {
            dest[i]  = (src[i] << 1) | ((src[i+1] & 0x80) >> 7);
        }

        for (i = 4; i < 16; i++)
        {
            dest[i] = 0;
        }
    } else {
        /*
         * FALSE means IPv6
         */

        addr->is_ipv4 = FALSE;

        /*
         * Copying the bits into the IP address is a pain since
         * everything is off by one bit.  Joy.
         */

        guint8  *src = exp->values->data;
        guint8  *dest = (guint8 *) addr->addr;

        gint  i;

        for (i = 0; i < 16; i++)
        {
            dest[i]  = (src[i] << 1) | ((src[i+1] & 0x80) >> 7);
        }
    }

    g_d_debug("Current IP address is %s",
              ipset_ip_to_string(addr));
}


/**
 * Process the current expanded assignment in the current BDD
 * assignment.
 */

static void
process_expanded_assignment(ipset_iterator_t *iterator)
{
    if (iterator->assignment_iterator->finished)
    {
        /*
         * If there isn't anything in the expanded assignment, advance
         * to the next BDD assignment.
         */

        g_d_debug("Expanded assignment is finished");

        ipset_expanded_assignment_free(iterator->assignment_iterator);
        iterator->assignment_iterator = NULL;

        ipset_bdd_iterator_advance(iterator->bdd_iterator);
        process_assignment(iterator);

    } else {
        /*
         * Otherwise, we've found a fully expanded assignment, so
         * create an IP address for it and return.
         */

        create_ip_address(&iterator->addr,
                          iterator->assignment_iterator);
    }
}


/**
 * Process the current assignment in the BDD iterator.
 */

static void
process_assignment(ipset_iterator_t *iterator)
{
    while (!iterator->bdd_iterator->finished)
    {
        if (iterator->bdd_iterator->value ==
            iterator->desired_value)
        {
            /*
             * If the BDD iterator hasn't finished, and the result of
             * the function with this assignment matches what the
             * caller wants, then we've found an assignment to
             * generate IP addresses from.
             *
             * Try to expand this assignment, and process the first
             * expanded assignment.  We want 32 + 1 variables if the
             * current address is IPv4; 128 + 1 if it's IPv6.
             */

            g_d_debug("Got a matching BDD assignment");

            ipset_tribool_t  address_type = ipset_assignment_get
                (iterator->bdd_iterator->assignment, 0);

            if (address_type == IPSET_FALSE)
            {
                /*
                 * FALSE means IPv6
                 */

                g_d_debug("Assignment is IPv6");

                iterator->assignment_iterator =
                    ipset_assignment_expand
                    (iterator->bdd_iterator->assignment,
                     IPV6_BIT_SIZE + 1);

            } else if (address_type == IPSET_TRUE) {
                /*
                 * TRUE means IPv4
                 */

                g_d_debug("Assignment is IPv4");

                iterator->assignment_iterator =
                    ipset_assignment_expand
                    (iterator->bdd_iterator->assignment,
                     IPV4_BIT_SIZE + 1);

            } else {
                /*
                 * Oof.
                 */

                g_d_debug("Assignment is both IPv4 and IPv6...uh-oh");

                g_assert(FALSE);
            }

            return process_expanded_assignment(iterator);
        }

        /*
         * The BDD iterator has a value, but it doesn't match the one
         * we want.  Advance the BDD iterator and try again.
         */

        g_d_debug("Value is %d, skipping",
                  iterator->bdd_iterator->value);
        ipset_bdd_iterator_advance(iterator->bdd_iterator);
    }

    /*
     * If we fall through, then the BDD iterator has finished.  That
     * means there's nothing left for the set iterator.
     */

    g_d_debug("Set iterator is finished");

    ipset_expanded_assignment_free(iterator->assignment_iterator);
    iterator->assignment_iterator = NULL;

    ipset_bdd_iterator_free(iterator->bdd_iterator);
    iterator->bdd_iterator = NULL;

    iterator->finished = TRUE;
}


ipset_iterator_t *
ipset_iterate(ip_set_t *set, gboolean desired_value)
{
    /*
     * First allocate the iterator itself.
     */

    ipset_iterator_t  *iterator;

    iterator = g_slice_new(ipset_iterator_t);
    iterator->finished = FALSE;
    iterator->assignment_iterator = NULL;
    iterator->desired_value = desired_value;

    /*
     * Then create the iterator that returns each BDD assignment.
     */

    g_d_debug("Iterating set");
    iterator->bdd_iterator = ipset_node_iterate(set->set_bdd);

    /*
     * Then drill down from the current BDD assignment, creating an
     * expanded assignment for it.
     */

    process_assignment(iterator);
    return iterator;
}


void
ipset_iterator_free(ipset_iterator_t *iterator)
{
    if (iterator == NULL)
        return;

    ipset_bdd_iterator_free(iterator->bdd_iterator);
    ipset_expanded_assignment_free(iterator->assignment_iterator);
    g_slice_free(ipset_iterator_t, iterator);
}


void
ipset_iterator_advance(ipset_iterator_t *iterator)
{
    /*
     * If we're already at the end of the iterator, don't do anything.
     */

    if (G_UNLIKELY(iterator->finished))
    {
        return;
    }

    /*
     * Otherwise, advance the expanded assignment iterator to the next
     * assignment, and then drill down into it.
     */

    g_d_debug("Advancing set iterator");

    ipset_expanded_assignment_advance(iterator->assignment_iterator);
    process_expanded_assignment(iterator);
}
