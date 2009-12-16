/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IPSET_IPSET_H
#define IPSET_IPSET_H

#include <bdd.h>


typedef struct ipv4_set
{
    BDD  set_bdd;
} ipv4_set_t;


typedef struct ipv6_set
{
    BDD  set_bdd;
} ipv6_set_t;


/*---------------------------------------------------------------------
 * General functions
 */

/**
 * Initializes the library.  Must be called before any other ipset
 * function.  Can safely be called multiple times.
 */

int ipset_init();


/*---------------------------------------------------------------------
 * IPv4 functions
 */

/**
 * Initializes a new IPv4 set that has already been allocated (on the
 * stack, for instance).  After returning, the set will be empty.
 */

void
ipset_init_ipv4_set(ipv4_set_t *set);

/**
 * Finalize an IPv4 set, freeing any space used to represent the set
 * internally.  Doesn't deallocate the ipv4_set_t itself, so this is
 * safe to call on stack-allocated sets.
 */

void
ipset_done_ipv4_set(ipv4_set_t *set);

/**
 * Creates a new empty IPv4 set on the heap.  Returns NULL if we can't
 * allocate a new instance.
 */

ipv4_set_t *
ipset_new_ipv4_set();

/**
 * Finalize and free a heap-allocated IPv4 set, freeing any space used
 * to represent the set internally.
 */

void
ipset_free_ipv4_set(ipv4_set_t *set);


/*---------------------------------------------------------------------
 * IPv6 functions
 */

/**
 * Initializes a new IPv6 set that has already been allocated (on the
 * stack, for instance).  After returning, the set will be empty.
 */

void
ipset_init_ipv6_set(ipv6_set_t *set);

/**
 * Finalize an IPv6 set, freeing any space used to represent the set
 * internally.  Doesn't deallocate the ipv6_set_t itself, so this is
 * safe to call on stack-allocated sets.
 */

void
ipset_done_ipv6_set(ipv6_set_t *set);

/**
 * Creates a new empty IPv6 set on the heap.  Returns NULL if we can't
 * allocate a new instance.
 */

ipv6_set_t *
ipset_new_ipv6_set();

/**
 * Finalize and free a heap-allocated IPv6 set, freeing any space used
 * to represent the set internally.
 */

void
ipset_free_ipv6_set(ipv6_set_t *set);


#endif  /* IPSET_IPSET_H */
