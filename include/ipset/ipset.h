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
ipset_ipv4_init(ipv4_set_t *set);

/**
 * Finalize an IPv4 set, freeing any space used to represent the set
 * internally.  Doesn't deallocate the ipv4_set_t itself, so this is
 * safe to call on stack-allocated sets.
 */

void
ipset_ipv4_done(ipv4_set_t *set);

/**
 * Creates a new empty IPv4 set on the heap.  Returns NULL if we can't
 * allocate a new instance.
 */

ipv4_set_t *
ipset_ipv4_new();

/**
 * Finalize and free a heap-allocated IPv4 set, freeing any space used
 * to represent the set internally.
 */

void
ipset_ipv4_free(ipv4_set_t *set);


/*---------------------------------------------------------------------
 * IPv6 functions
 */

/**
 * Initializes a new IPv6 set that has already been allocated (on the
 * stack, for instance).  After returning, the set will be empty.
 */

void
ipset_ipv6_init(ipv6_set_t *set);

/**
 * Finalize an IPv6 set, freeing any space used to represent the set
 * internally.  Doesn't deallocate the ipv6_set_t itself, so this is
 * safe to call on stack-allocated sets.
 */

void
ipset_ipv6_done(ipv6_set_t *set);

/**
 * Creates a new empty IPv6 set on the heap.  Returns NULL if we can't
 * allocate a new instance.
 */

ipv6_set_t *
ipset_ipv6_new();

/**
 * Finalize and free a heap-allocated IPv6 set, freeing any space used
 * to represent the set internally.
 */

void
ipset_ipv6_free(ipv6_set_t *set);


#endif  /* IPSET_IPSET_H */
