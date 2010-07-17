/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IPSET_IPSET_H
#define IPSET_IPSET_H

#include <stdio.h>

#include <glib.h>
#include <gio/gio.h>

#include <ipset/internal.h>


typedef struct ip_set
{
    ipset_node_id_t  set_bdd;
} ip_set_t;


typedef struct ip_map
{
    ipset_node_id_t  map_bdd;
    ipset_node_id_t  default_bdd;
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

gboolean
ipset_is_empty(ip_set_t *set);

/**
 * Returns whether two IP sets are equal.
 */

gboolean
ipset_is_equal(ip_set_t *set1, ip_set_t *set2);

/**
 * Returns whether two IP sets are not equal.
 */

gboolean
ipset_is_not_equal(ip_set_t *set1, ip_set_t *set2);

/**
 * Returns the number of bytes needed to store the IP set.  Note that
 * adding together the storage needed for each set you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between sets.
 */

gsize
ipset_memory_size(ip_set_t *set);

/**
 * Saves an IP set to disk.  Returns a boolean indicating whether the
 * operation was successful.
 */

gboolean
ipset_save(GOutputStream *stream,
           ip_set_t *set,
           GError **err);

/**
 * Loads an IP set from a stream.  Returns NULL if the set cannot be
 * loaded.
 */

ip_set_t *
ipset_load(GInputStream *stream,
           GError **err);

/**
 * Adds a single IPv4 address to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

gboolean
ipset_ipv4_add(ip_set_t *set, gpointer elem);

/**
 * Adds a network of IPv4 addresses to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the set.
 *
 * Returns whether the network was already in the set or not.
 */

gboolean
ipset_ipv4_add_network(ip_set_t *set, gpointer elem, guint netmask);

/**
 * Adds a single IPv6 address to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 128-bit big-endian integer.
 *
 * Returns whether the value was already in the set or not.
 */

gboolean
ipset_ipv6_add(ip_set_t *set, gpointer elem);

/**
 * Adds a network of IPv6 addresses to an IP set.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 128-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the set.
 *
 * Returns whether the network was already in the set or not.
 */

gboolean
ipset_ipv6_add_network(ip_set_t *set, gpointer elem, guint netmask);


/*---------------------------------------------------------------------
 * IP map functions
 */

/**
 * Initializes a new IP map that has already been allocated (on the
 * stack, for instance).  After returning, the map will be empty.  Any
 * addresses that aren't explicitly added to the map will have
 * default_value as their value.
 */

void
ipmap_init(ip_map_t *map, gint default_value);

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
 * added to the map will have default_value as their value.
 */

ip_map_t *
ipmap_new(gint default_value);

/**
 * Finalize and free a heap-allocated IP map, freeing any space used
 * to represent the map internally.
 */

void
ipmap_free(ip_map_t *map);

/**
 * Returns whether the IP map is empty.  A map is considered empty if
 * every input is mapped to the default value.
 */

gboolean
ipmap_is_empty(ip_map_t *map);

/**
 * Returns whether two IP maps are equal.
 */

gboolean
ipmap_is_equal(ip_map_t *map1, ip_map_t *map2);

/**
 * Returns whether two IP maps are not equal.
 */

gboolean
ipmap_is_not_equal(ip_map_t *map1, ip_map_t *map2);

/**
 * Returns the number of bytes needed to store the IP map.  Note that
 * adding together the storage needed for each map you use doesn't
 * necessarily give you the total memory requirements, since some
 * storage can be shared between maps.
 */

gsize
ipmap_memory_size(ip_map_t *map);

/**
 * Saves an IP map to disk.  Returns a boolean indicating whether the
 * operation was successful.
 */

gboolean
ipmap_save(GOutputStream *stream,
           ip_map_t *map,
           GError **err);

/**
 * Loads an IP map from disk.  Returns NULL if the map cannot be
 * loaded.
 */

ip_map_t *
ipmap_load(GInputStream *stream,
           GError **err);

/**
 * Adds a single IPv4 address to an IP map, with the given value.  We
 * don't care what specific type is used to represent the address;
 * elem should be a pointer to an address stored as a 32-bit
 * big-endian integer.
 */

void
ipmap_ipv4_set(ip_map_t *map, gpointer elem, gint value);

/**
 * Adds a network of IPv4 addresses to an IP map, with each address in
 * the network mapping to the given value.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 32-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the map.
 */

void
ipmap_ipv4_set_network(ip_map_t *map,
                       gpointer elem,
                       guint netmask,
                       gint value);

/**
 * Returns the value that an IPv4 address is mapped to in the map.  We
 * don't care what specific type is used to represent the address;
 * elem should be a pointer to an address stored as a 32-bit
 * big-endian integer.
 */

gint
ipmap_ipv4_get(ip_map_t *map, gpointer elem);

/**
 * Adds a single IPv6 address to an IP map, with the given value.  We
 * don't care what specific type is used to represent the address;
 * elem should be a pointer to an address stored as a 128-bit
 * big-endian integer.
 */

void
ipmap_ipv6_set(ip_map_t *map, gpointer elem, gint value);

/**
 * Adds a network of IPv6 addresses to an IP map, with each address in
 * the network mapping to the given value.  We don't care what
 * specific type is used to represent the address; elem should be a
 * pointer to an address stored as a 128-bit big-endian integer.  All
 * of the addresses that start with the first netmask bits of elem
 * will be added to the map.
 */

void
ipmap_ipv6_set_network(ip_map_t *map,
                       gpointer elem,
                       guint netmask,
                       gint value);

/**
 * Returns the value that an IPv6 address is mapped to in the map.  We
 * don't care what specific type is used to represent the address;
 * elem should be a pointer to an address stored as a 128-bit
 * big-endian integer.
 */

gint
ipmap_ipv6_get(ip_map_t *map, gpointer elem);


#endif  /* IPSET_IPSET_H */
