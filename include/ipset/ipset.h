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


typedef struct ip_set
{
    DdNode  *set_bdd;
} ip_set_t;


typedef struct ip_map
{
    DdNode  *map_add;
    DdNode  *default_add;
} ip_map_t;


/*---------------------------------------------------------------------
 * General functions
 */

/**
 * Initializes the library.  Must be called before any other ipset
 * function.  Can safely be called multiple times.
 */

int ipset_init_library();


/*---------------------------------------------------------------------
 * IP set functions
 */

/**
 * Initializes a new IP set that has already been allocated (on the
 * stack, for instance).  After returning, the set will be empty.
 */

void
ipset_init(ip_set_t *set);

/**
 * Finalize an IP set, freeing any space used to represent the set
 * internally.  Doesn't deallocate the ip_set_t itself, so this is
 * safe to call on stack-allocated sets.
 */

void
ipset_done(ip_set_t *set);

/**
 * Creates a new empty IP set on the heap.  Returns NULL if we can't
 * allocate a new instance.
 */

ip_set_t *
ipset_new();

/**
 * Finalize and free a heap-allocated IP set, freeing any space used
 * to represent the set internally.
 */

void
ipset_free(ip_set_t *set);

/**
 * Returns whether the IP set is empty.
 */

bool
ipset_is_empty(ip_set_t *set);

/**
 * Returns whether two IP sets are equal.
 */

bool
ipset_is_equal(ip_set_t *set1, ip_set_t *set2);

/**
 * Returns whether two IP sets are not equal.
 */

bool
ipset_is_not_equal(ip_set_t *set1, ip_set_t *set2);

/**
 * Returns the number of bytes needed to store the IP set.  Note that
 * adding together the storage needed for each set you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between sets.
 */

size_t
ipset_memory_size(ip_set_t *set);

/**
 * Saves an IP set to disk.  Returns a boolean indicating whether the
 * operation was successful.  The caller is responsible for opening
 * and closing the stream.
 */

bool
ipset_save(ip_set_t *set, FILE *file);

/**
 * Loads an IP set from disk.  Returns NULL if the set cannot be
 * loaded.  The caller is responsible for opening and closing the
 * stream.
 */

ip_set_t *
ipset_load(FILE *file);

/**
 * Adds a single IPv4 address to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

bool
ipset_ipv4_add(ip_set_t *set, void *elem);

/**
 * Adds a network of IPv4 addresses to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the set.
 *
 * Returns whether the network was already in the set or not.
 */

bool
ipset_ipv4_add_network(ip_set_t *set, void *elem, int netmask);

/**
 * Adds a single IPv6 address to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 128-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

bool
ipset_ipv6_add(ip_set_t *set, void *elem);

/**
 * Adds a network of IPv6 addresses to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 128-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the set.
 *
 * Returns whether the network was already in the set or not.
 */

bool
ipset_ipv6_add_network(ip_set_t *set, void *elem, int netmask);


/*---------------------------------------------------------------------
 * IP map functions
 */

/**
 * Initializes a new IP map that has already been allocated (on the
 * stack, for instance).  After returning, the map will be empty.  Any
 * addresses that aren't explicitly added to the map will have
 * default_value as their value.  The default value is specified as an
 * integer.
 */

void
ipmap_init(ip_map_t *map, intptr_t default_value);

/**
 * Initializes a new IP map that has already been allocated (on the
 * stack, for instance).  After returning, the map will be empty.  Any
 * addresses that aren't explicitly added to the map will have
 * default_value as their value.  The default value is specified as an
 * pointer.
 */

void
ipmap_init_ptr(ip_map_t *map, void *default_value);

/**
 * Finalize an IP map, freeing any space used to represent the map
 * internally.  Doesn't deallocate the ip_map_t itself, so this is
 * safe to call on stack-allocated maps.
 */

void
ipmap_done(ip_map_t *map);

/**
 * Creates a new empty IP map on the heap.  Returns NULL if we can't
 * allocate a new instance.  Any addresses that aren't explicitly
 * added to the map will have default_value as their value.  The
 * default value is specified as an integer.
 */

ip_map_t *
ipmap_new(intptr_t default_value);

/**
 * Creates a new empty IP map on the heap.  Returns NULL if we can't
 * allocate a new instance.  Any addresses that aren't explicitly
 * added to the map will have default_value as their value.  The
 * default value is specified as an pointer.
 */

ip_map_t *
ipmap_new_ptr(void *default_value);

/**
 * Finalize and free a heap-allocated IP map, freeing any space used
 * to represent the map internally.
 */

void
ipmap_free(ip_map_t *map);

/**
 * Returns whether the IP map is empty.
 */

bool
ipmap_is_empty(ip_map_t *map);

/**
 * Returns whether two IP maps are equal.
 */

bool
ipmap_is_equal(ip_map_t *map1, ip_map_t *map2);

/**
 * Returns whether two IP maps are not equal.
 */

bool
ipmap_is_not_equal(ip_map_t *map1, ip_map_t *map2);

/**
 * Returns the number of bytes needed to store the IP map.  Note that
 * adding together the storage needed for each map you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between maps.
 */

size_t
ipmap_memory_size(ip_map_t *map);

/**
 * Saves an IP map to disk.  Returns a boolean indicating whether the
 * operation was successful.  The caller is responsible for opening
 * and closing the stream.
 */

bool
ipmap_save(ip_map_t *map, FILE *file);

/**
 * Loads an IP map from disk.  Returns NULL if the map cannot be
 * loaded.  The caller is responsible for opening and closing the
 * stream.
 */

ip_map_t *
ipmap_load(FILE *file);

/**
 * Adds a single IPv4 address to an IP map, with the given value.  The
 * value is specified as an integer.  We don't care what specific type
 * is used to represent the address; elem should be a pointer to an
 * address stored as a 32-bit big-endian integer.
 */

void
ipmap_ipv4_set(ip_map_t *map, void *elem, intptr_t value);

/**
 * Adds a network of IPv4 addresses to an IP map, with each address in
 * the network mapping to the given value.  The value is specified as
 * an integer.  We don't care what specific type is used to represent
 * the address; elem should be a pointer to an address stored as a
 * 32-bit big-endian integer.  All of the addresses that start with
 * the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv4_set_network(ip_map_t *map,
                       void *elem,
                       int netmask,
                       intptr_t value);

/**
 * Adds a single IPv4 address to an IP map, with the given value.  The
 * value is specified as a pointer.  We don't care what specific type
 * is used to represent the address; elem should be a pointer to an
 * address stored as a 32-bit big-endian integer.
 */

void
ipmap_ipv4_set_ptr(ip_map_t *map, void *elem, void *value);

/**
 * Adds a network of IPv4 addresses to an IP map, with each address in
 * the network mapping to the given value.  The value is specified as
 * a pointer.  We don't care what specific type is used to represent
 * the address; elem should be a pointer to an address stored as a
 * 32-bit big-endian integer.  All of the addresses that start with
 * the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv4_set_network_ptr(ip_map_t *map,
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
ipmap_ipv4_get(ip_map_t *map, void *elem);

/**
 * Returns the value that an IPv4 address is mapped to in the map.
 * The value is returned as a pointer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 32-bit big-endian integer.
 */

void *
ipmap_ipv4_get_ptr(ip_map_t *map, void *elem);

/**
 * Adds a single IPv6 address to an IP map, with the given value.  The
 * value is specified as an integer.  We don't care what specific type
 * is used to represent the address; elem should be a pointer to an
 * address stored as a 128-bit big-endian integer.
 */

void
ipmap_ipv6_set(ip_map_t *map, void *elem, intptr_t value);

/**
 * Adds a network of IPv6 addresses to an IP map, with each address in
 * the network mapping to the given value.  The value is specified as
 * an integer.  We don't care what specific type is used to represent
 * the address; elem should be a pointer to an address stored as a
 * 128-bit big-endian integer.  All of the addresses that start with
 * the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv6_set_network(ip_map_t *map,
                       void *elem,
                       int netmask,
                       intptr_t value);

/**
 * Adds a single IPv6 address to an IP map, with the given value.  The
 * value is specified as a pointer.  We don't care what specific type
 * is used to represent the address; elem should be a pointer to an
 * address stored as a 128-bit big-endian integer.
 */

void
ipmap_ipv6_set_ptr(ip_map_t *map, void *elem, void *value);

/**
 * Adds a network of IPv6 addresses to an IP map, with each address in
 * the network mapping to the given value.  The value is specified as
 * a pointer.  We don't care what specific type is used to represent
 * the address; elem should be a pointer to an address stored as a
 * 128-bit big-endian integer.  All of the addresses that start with
 * the first netmask bits of elem will be added to the map.
 */

void
ipmap_ipv6_set_network_ptr(ip_map_t *map,
                           void *elem,
                           int netmask,
                           void *value);

/**
 * Returns the value that an IPv6 address is mapped to in the map.
 * The value is returned as an integer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 128-bit big-endian integer.
 */

intptr_t
ipmap_ipv6_get(ip_map_t *map, void *elem);

/**
 * Returns the value that an IPv6 address is mapped to in the map.
 * The value is returned as a pointer.  We don't care what specific
 * type is used to represent the address; elem should be a pointer to
 * an address stored as a 128-bit big-endian integer.
 */

void *
ipmap_ipv6_get_ptr(ip_map_t *map, void *elem);


#endif  /* IPSET_IPSET_H */
