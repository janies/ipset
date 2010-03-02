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
#include <stdint.h>
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


typedef struct ipv4_map
{
    DdNode  *map_add;
    DdNode  *default_add;
} ipv4_map_t;


typedef struct ipv6_map
{
    DdNode  *map_add;
    DdNode  *default_add;
} ipv6_map_t;


/*---------------------------------------------------------------------
 * General functions
 */

/**
 * Initializes the library.  Must be called before any other ipset
 * function.  Can safely be called multiple times.
 */

int ipset_init();


/*---------------------------------------------------------------------
 * IPv4 set functions
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
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

bool
ipset_ipv4_add(ipv4_set_t *set, void *elem);

/**
 * Adds a network of IPv4 addresses to an IPv4 set.  We don't care
 * what specific type is used to represent the address; elem should be
 * a pointer to an address stored as a 32-bit big-endian integer.  All
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
 * IPv6 set functions
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
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 128-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

bool
ipset_ipv6_add(ipv6_set_t *set, void *elem);

/**
 * Adds a network of IPv6 addresses to an IPv6 set.  We don't care
 * what specific type is used to represent the address; elem should be
 * a pointer to an address stored as a 32-bit big-endian integer.  All
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


/*---------------------------------------------------------------------
 * IPv4 map functions
 */

/**
 * Initializes a new IPv4 map that has already been allocated (on the
 * stack, for instance).  After returning, the map will be empty.  Any
 * addresses that aren't explicitly added to the map will have
 * default_value as their value.  The default value is specified as an
 * integer.
 */

void
ipmap_ipv4_init(ipv4_map_t *map, intptr_t default_value);

/**
 * Initializes a new IPv4 map that has already been allocated (on the
 * stack, for instance).  After returning, the map will be empty.  Any
 * addresses that aren't explicitly added to the map will have
 * default_value as their value.  The default value is specified as an
 * pointer.
 */

void
ipmap_ipv4_init_ptr(ipv4_map_t *map, void *default_value);

/**
 * Finalize an IPv4 map, freeing any space used to represent the map
 * internally.  Doesn't deallocate the ipv4_map_t itself, so this is
 * safe to call on stack-allocated maps.
 */

void
ipmap_ipv4_done(ipv4_map_t *map);

/**
 * Creates a new empty IPv4 map on the heap.  Returns NULL if we can't
 * allocate a new instance.  Any addresses that aren't explicitly
 * added to the map will have default_value as their value.  The
 * default value is specified as an integer.
 */

ipv4_map_t *
ipmap_ipv4_new(intptr_t default_value);

/**
 * Creates a new empty IPv4 map on the heap.  Returns NULL if we can't
 * allocate a new instance.  Any addresses that aren't explicitly
 * added to the map will have default_value as their value.  The
 * default value is specified as an pointer.
 */

ipv4_map_t *
ipmap_ipv4_new_ptr(void *default_value);

/**
 * Finalize and free a heap-allocated IPv4 map, freeing any space used
 * to represent the map internally.
 */

void
ipmap_ipv4_free(ipv4_map_t *map);

/**
 * Returns whether the IPv4 map is empty.
 */

bool
ipmap_ipv4_is_empty(ipv4_map_t *map);

/**
 * Returns whether two IPv4 maps are equal.
 */

bool
ipmap_ipv4_is_equal(ipv4_map_t *map1, ipv4_map_t *map2);

/**
 * Returns whether two IPv4 maps are not equal.
 */

bool
ipmap_ipv4_is_not_equal(ipv4_map_t *map1, ipv4_map_t *map2);

/**
 * Adds a single IPv4 address to an IPv4 map, with the given value.
 * The value is specified as an integer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

void
ipmap_ipv4_set(ipv4_map_t *map, void *elem, intptr_t value);

/**
 * Adds a network of IPv4 addresses to an IPv4 map, with each address
 * in the network mapping to the given value.  The value is specified
 * as an integer.  We don't care what specific type is used to
 * represent the address; elem should be a pointer to an address
 * stored as a 32-bit big-endian integer.  All of the addresses that
 * start with the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv4_set_network(ipv4_map_t *map,
                       void *elem,
                       int netmask,
                       intptr_t value);

/**
 * Adds a single IPv4 address to an IPv4 map, with the given value.
 * The value is specified as a pointer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

void
ipmap_ipv4_set_ptr(ipv4_map_t *map, void *elem, void *value);

/**
 * Adds a network of IPv4 addresses to an IPv4 map, with each address
 * in the network mapping to the given value.  The value is specified
 * as a pointer.  We don't care what specific type is used to
 * represent the address; elem should be a pointer to an address
 * stored as a 32-bit big-endian integer.  All of the addresses that
 * start with the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv4_set_network_ptr(ipv4_map_t *map,
                           void *elem,
                           int netmask,
                           void *value);

/**
 * Returns the value that an IPv4 address is mapped to in the map.
 * The value is returned as an integer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

intptr_t
ipmap_ipv4_get(ipv4_map_t *map, void *elem);

/**
 * Returns the value that an IPv4 address is mapped to in the map.
 * The value is returned as a pointer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

void *
ipmap_ipv4_get_ptr(ipv4_map_t *map, void *elem);

/**
 * Returns the number of bytes needed to store the IP map.  Note that
 * adding together the storage needed for each map you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between maps.
 */

size_t
ipmap_ipv4_memory_size(ipv4_map_t *map);


/*---------------------------------------------------------------------
 * IPv6 map functions
 */

/**
 * Initializes a new IPv6 map that has already been allocated (on the
 * stack, for instance).  After returning, the map will be empty.  Any
 * addresses that aren't explicitly added to the map will have
 * default_value as their value.  The default value is specified as an
 * integer.
 */

void
ipmap_ipv6_init(ipv6_map_t *map, intptr_t default_value);

/**
 * Initializes a new IPv6 map that has already been allocated (on the
 * stack, for instance).  After returning, the map will be empty.  Any
 * addresses that aren't explicitly added to the map will have
 * default_value as their value.  The default value is specified as an
 * pointer.
 */

void
ipmap_ipv6_init_ptr(ipv6_map_t *map, void *default_value);

/**
 * Finalize an IPv6 map, freeing any space used to represent the map
 * internally.  Doesn't deallocate the ipv6_map_t itself, so this is
 * safe to call on stack-allocated maps.
 */

void
ipmap_ipv6_done(ipv6_map_t *map);

/**
 * Creates a new empty IPv6 map on the heap.  Returns NULL if we can't
 * allocate a new instance.  Any addresses that aren't explicitly
 * added to the map will have default_value as their value.  The
 * default value is specified as an integer.
 */

ipv6_map_t *
ipmap_ipv6_new(intptr_t default_value);

/**
 * Creates a new empty IPv6 map on the heap.  Returns NULL if we can't
 * allocate a new instance.  Any addresses that aren't explicitly
 * added to the map will have default_value as their value.  The
 * default value is specified as an pointer.
 */

ipv6_map_t *
ipmap_ipv6_new_ptr(void *default_value);

/**
 * Finalize and free a heap-allocated IPv6 map, freeing any space used
 * to represent the map internally.
 */

void
ipmap_ipv6_free(ipv6_map_t *map);

/**
 * Returns whether the IPv6 map is empty.
 */

bool
ipmap_ipv6_is_empty(ipv6_map_t *map);

/**
 * Returns whether two IPv6 maps are equal.
 */

bool
ipmap_ipv6_is_equal(ipv6_map_t *map1, ipv6_map_t *map2);

/**
 * Returns whether two IPv6 maps are not equal.
 */

bool
ipmap_ipv6_is_not_equal(ipv6_map_t *map1, ipv6_map_t *map2);

/**
 * Adds a single IPv6 address to an IPv6 map, with the given value.
 * The value is specified as an integer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

void
ipmap_ipv6_set(ipv6_map_t *map, void *elem, intptr_t value);

/**
 * Adds a network of IPv6 addresses to an IPv6 map, with each address
 * in the network mapping to the given value.  The value is specified
 * as an integer.  We don't care what specific type is used to
 * represent the address; elem should be a pointer to an address
 * stored as a 32-bit big-endian integer.  All of the addresses that
 * start with the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv6_set_network(ipv6_map_t *map,
                       void *elem,
                       int netmask,
                       intptr_t value);

/**
 * Adds a single IPv6 address to an IPv6 map, with the given value.
 * The value is specified as a pointer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

void
ipmap_ipv6_set_ptr(ipv6_map_t *map, void *elem, void *value);

/**
 * Adds a network of IPv6 addresses to an IPv6 map, with each address
 * in the network mapping to the given value.  The value is specified
 * as a pointer.  We don't care what specific type is used to
 * represent the address; elem should be a pointer to an address
 * stored as a 32-bit big-endian integer.  All of the addresses that
 * start with the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv6_set_network_ptr(ipv6_map_t *map,
                           void *elem,
                           int netmask,
                           void *value);

/**
 * Returns the value that an IPv6 address is mapped to in the map.
 * The value is returned as an integer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

intptr_t
ipmap_ipv6_get(ipv6_map_t *map, void *elem);

/**
 * Returns the value that an IPv6 address is mapped to in the map.
 * The value is returned as a pointer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

void *
ipmap_ipv6_get_ptr(ipv6_map_t *map, void *elem);

/**
 * Returns the number of bytes needed to store the IP map.  Note that
 * adding together the storage needed for each map you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between maps.
 */

size_t
ipmap_ipv6_memory_size(ipv6_map_t *map);


#endif  /* IPSET_IPSET_H */
