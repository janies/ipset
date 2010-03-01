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

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <cudd.h>


typedef struct ipv4_set
{
    DdNode  *set_bdd;
} ipv4_set_t;


typedef struct ipv6_set
{
    DdNode  *set_bdd;
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

/**
 * Returns whether the IPv4 set is empty.
 */

bool
ipset_ipv4_is_empty(ipv4_set_t *set);

/**
 * Returns whether two IPv4 sets are equal.
 */

bool
ipset_ipv4_is_equal(ipv4_set_t *set1, ipv4_set_t *set2);

/**
 * Returns whether two IPv4 sets are not equal.
 */

bool
ipset_ipv4_is_not_equal(ipv4_set_t *set1, ipv4_set_t *set2);

/**
 * Adds a single IPv4 address to an IPv4 set.  We don't care what
 * specific type is used to represent address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

bool
ipset_ipv4_add(ipv4_set_t *set, void *elem);

/**
 * Adds a network of IPv4 addresses to an IPv4 set.  We don't care
 * what specific type is used to represent address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the set.
 *
 * Returns whether the network was already in the set or not.
 */

bool
ipset_ipv4_add_network(ipv4_set_t *set, void *elem, int netmask);

/**
 * Returns the number of bytes needed to store the IP set.  Note that
 * adding together the storage needed for each set you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between sets.
 */

size_t
ipset_ipv4_memory_size(ipv4_set_t *set);


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

/**
 * Returns whether the IPv6 set is empty.
 */

bool
ipset_ipv6_is_empty(ipv6_set_t *set);

/**
 * Returns whether two IPv6 sets are equal.
 */

bool
ipset_ipv6_is_equal(ipv6_set_t *set1, ipv6_set_t *set2);

/**
 * Returns whether two IPv6 sets are not equal.
 */

bool
ipset_ipv6_is_not_equal(ipv6_set_t *set1, ipv6_set_t *set2);

/**
 * Adds a single IPv6 address to an IPv6 set.  We don't care what
 * specific type is used to represent address; elem should be a
 * pointer to an address stored as a 128-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

bool
ipset_ipv6_add(ipv6_set_t *set, void *elem);

/**
 * Adds a network of IPv6 addresses to an IPv6 set.  We don't care
 * what specific type is used to represent address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the set.
 *
 * Returns whether the network was already in the set or not.
 */

bool
ipset_ipv6_add_network(ipv6_set_t *set, void *elem, int netmask);

/**
 * Returns the number of bytes needed to store the IP set.  Note that
 * adding together the storage needed for each set you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between sets.
 */

size_t
ipset_ipv6_memory_size(ipv6_set_t *set);


#endif  /* IPSET_IPSET_H */
