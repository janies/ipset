/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IPSET_IP_H
#define IPSET_IP_H

#include <glib.h>


/**
 * A discriminated IP address.  The address can either be IPv4 or
 * IPv6.
 */

typedef struct ipset_ip
{
    /**
     * The content of the IP address.  This contains enough data to
     * hold either an IPv4 or IPv6 address.  If the address is IPv4,
     * the address data will be stored in the first guint32 value; the
     * remaining values must be initialized to 0.  If the address is
     * IPv6, all four guint32 values will be filled in.
     */

    guint32  addr[4];

    /**
     * A gboolean indicating which kind of address is stored: TRUE for
     * IPv4, FALSE for IPv6.
     */

    gboolean  is_ipv4;
} ipset_ip_t;


/**
 * Create an ipset_ip_t from an IPv4 address.
 */

void
ipset_ip_from_ipv4(ipset_ip_t *addr, void *v_ipv4);


/**
 * Create an ipset_ip_t from an IPv6 address.
 */

void
ipset_ip_from_ipv6(ipset_ip_t *addr, void *v_ipv6);


/**
 * Create an ipset_ip_t from another generic IP address.
 */

void
ipset_ip_copy(ipset_ip_t *addr, const ipset_ip_t *src);


/**
 * A hash function for ipset_ip_t objects.
 */

guint
ipset_ip_hash(ipset_ip_t *addr);


/**
 * Compare two ipset_ip_t objects for equality.
 */

gboolean
ipset_ip_equal(const ipset_ip_t *addr1,
               const ipset_ip_t *addr2);


/**
 * Create an ipset_ip_t from a string.  We first attempt to parse the
 * string as an IPv4 address; if that fails, we try parsing it as
 * IPv6.  If both attempts fail, we return FALSE.
 */

gboolean
ipset_ip_from_string(ipset_ip_t *addr, const gchar *str);


/**
 * Return a string containing a human-readable version of an
 * ipset_ip_t.  The string is not owned by the caller; it should not
 * be freed, and any later calls to this function will invalidate any
 * previous results.  If a result needs to be saved, it should be
 * duplicated using strdup or g_strdup.
 */

const gchar *
ipset_ip_to_string(const ipset_ip_t *addr);


#endif  /* IPSET_IP_H */
