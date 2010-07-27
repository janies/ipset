/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <string.h>

#include <glib.h>

#include <ipset/bdd/nodes.h>
#include <ipset/ipset.h>
#include <ipset/logging.h>


/* Forward declarations */

static void
process_assignment(ipset_iterator_t *iterator);

static void
expand_ipv6(ipset_iterator_t *iterator);


/**
 * Find the highest non-EITHER bit in an assignment, starting from the
 * given bit index.
 */

static guint
find_last_non_either_bit(ipset_assignment_t *assignment,
                         guint starting_bit)
{
    guint  i;

    for (i = starting_bit; i >= 1; i--)
    {
        ipset_tribool_t  value = ipset_assignment_get(assignment, i);
        if (value != IPSET_EITHER)
        {
            return i;
        }
    }

    return 0;
}


/**
 * Create a generic IP address object from the current expanded
 * assignment.
 */

static void
create_ip_address(ipset_iterator_t *iterator)
{
    ipset_ip_t  *addr = &iterator->addr;
    ipset_expanded_assignment_t  *exp =
        iterator->assignment_iterator;

    /*
     * Check variable 0 to see if this is an IPv4 or IPv6 address.
     */

    addr->is_ipv4 = IPSET_BIT_GET(exp->values->data, 0);

    /*
     * Initialize the address to all 0 bits.
     */

    memset(addr->addr, 0, sizeof(guint32) * 4);

    /*
     * Copy bits from the expanded assignment.  The number of bits to
     * copy is given as the current netmask.  We'll have calculated
     * that already based on the non-expanded assignment.
     */

    guint  i;
    for (i = 0; i < iterator->netmask; i++)
    {
        IPSET_BIT_SET(addr->addr, i,
                      IPSET_BIT_GET(exp->values->data, i+1));
    }

    g_d_debug("Current IP address is %s/%u",
              ipset_ip_to_string(addr), iterator->netmask);
}


/**
 * Advance the BDD iterator, taking into account that some assignments
 * need to be expanded twice.
 */

static void
advance_assignment(ipset_iterator_t *iterator)
{
    /*
     * Check the current state of the iterator to determine how to
     * advance.
     */

    /*
     * In most cases, the assignment we just finished only needed to
     * be expanded once.  So we move on to the next assignment and
     * process it.
     */

    if (G_LIKELY(iterator->multiple_expansion_state ==
                 IPSET_ITERATOR_NORMAL))
    {
        ipset_bdd_iterator_advance(iterator->bdd_iterator);
        process_assignment(iterator);
        return;
    }

    /*
     * If the assignment needs to be expanded twice, we'll do the IPv4
     * expansion first.  If that's what we've just finished, do the
     * IPv6 expansion next.
     */

    if (iterator->multiple_expansion_state ==
        IPSET_ITERATOR_MULTIPLE_IPV4)
    {
        g_d_debug("Expanding IPv6 second");

        iterator->multiple_expansion_state =
            IPSET_ITERATOR_MULTIPLE_IPV6;
        ipset_assignment_set
            (iterator->bdd_iterator->assignment,
             0, IPSET_FALSE);
        expand_ipv6(iterator);
        return;
    }

    /*
     * If we've just finished the IPv6 expansion, then we've finished
     * with this assignment.  Before moving on to the next one, we
     * have to reset variable 0 to EITHER (which it was before we
     * started this whole mess).
     */

    if (iterator->multiple_expansion_state ==
        IPSET_ITERATOR_MULTIPLE_IPV6)
    {
        g_d_debug("Finished both expansions");

        ipset_assignment_set
            (iterator->bdd_iterator->assignment,
             0, IPSET_EITHER);
        ipset_bdd_iterator_advance(iterator->bdd_iterator);
        process_assignment(iterator);
        return;
    }
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

        advance_assignment(iterator);
    } else {
        /*
         * Otherwise, we've found a fully expanded assignment, so
         * create an IP address for it and return.
         */

        create_ip_address(iterator);
    }
}


/**
 * Expand the current assignment as IPv4 addresses.
 */

static void
expand_ipv4(ipset_iterator_t *iterator)
{
    guint  last_bit;

    if (iterator->summarize)
    {
        last_bit = find_last_non_either_bit
            (iterator->bdd_iterator->assignment,
             IPV4_BIT_SIZE);

        g_d_debug("Last non-either bit is %u", last_bit);
    } else {
        last_bit = IPV4_BIT_SIZE;
    }

    iterator->assignment_iterator =
        ipset_assignment_expand
        (iterator->bdd_iterator->assignment,
         last_bit + 1);
    iterator->netmask = last_bit;

    process_expanded_assignment(iterator);
}


/**
 * Expand the current assignment as IPv4 addresses.
 */

static void
expand_ipv6(ipset_iterator_t *iterator)
{
    guint  last_bit;

    if (iterator->summarize)
    {
        last_bit = find_last_non_either_bit
            (iterator->bdd_iterator->assignment,
             IPV6_BIT_SIZE);

        g_d_debug("Last non-either bit is %u", last_bit);
    } else {
        last_bit = IPV6_BIT_SIZE;
    }

    iterator->assignment_iterator =
        ipset_assignment_expand
        (iterator->bdd_iterator->assignment,
         last_bit + 1);
    iterator->netmask = last_bit;

    process_expanded_assignment(iterator);
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
                iterator->multiple_expansion_state =
                    IPSET_ITERATOR_NORMAL;
                expand_ipv6(iterator);
                return;
            } else if (address_type == IPSET_TRUE) {
                /*
                 * TRUE means IPv4
                 */

                g_d_debug("Assignment is IPv4");
                iterator->multiple_expansion_state =
                    IPSET_ITERATOR_NORMAL;
                expand_ipv4(iterator);
                return;
            } else {
                /*
                 * EITHER means that this assignment contains both
                 * IPv4 and IPv6 addresses.  Expand it as IPv4 first.
                 */

                g_d_debug("Assignment is both IPv4 and IPv6");
                g_d_debug("Expanding IPv4 first");

                iterator->multiple_expansion_state =
                    IPSET_ITERATOR_MULTIPLE_IPV4;
                ipset_assignment_set
                    (iterator->bdd_iterator->assignment,
                     0, IPSET_TRUE);
                expand_ipv4(iterator);
                return;
            }
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


static ipset_iterator_t *
create_iterator(ip_set_t *set, gboolean desired_value,
                gboolean summarize)
{
    /*
     * First allocate the iterator itself.
     */

    ipset_iterator_t  *iterator;

    iterator = g_slice_new(ipset_iterator_t);
    iterator->finished = FALSE;
    iterator->assignment_iterator = NULL;
    iterator->desired_value = desired_value;
    iterator->summarize = summarize;

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


ipset_iterator_t *
ipset_iterate(ip_set_t *set, gboolean desired_value)
{
    return create_iterator(set, desired_value, FALSE);
}


ipset_iterator_t *
ipset_iterate_networks(ip_set_t *set, gboolean desired_value)
{
    return create_iterator(set, desired_value, TRUE);
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
