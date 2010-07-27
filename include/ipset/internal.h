/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IPSET_INTERNAL_H
#define IPSET_INTERNAL_H

#include <ipset/bdd/nodes.h>
#include <ipset/ipset.h>


/*
 * The BDD node cache for the IP set functions.
 */

extern ipset_node_cache_t *ipset_cache;


/**
 * Create a BDD for an IP address or family of IP addresses.  The
 * corresponding BDD will have each boolean variable set if the
 * corresponding bit is set in the IP address.  The netmask parameter
 * can be used to limit the number of bits to constrain; if this is
 * less than IPVX_BIT_SIZE, then an entire network will be added to
 * the set.  The values of the BDD will all be 0 or 1, so the BDD is
 * acceptable to pass in as the condition in a call to
 * ipset_node_cache_ite().
 */

ipset_node_id_t
ipset_ipv4_make_ip_bdd(gpointer addr, guint netmask);

ipset_node_id_t
ipset_ipv6_make_ip_bdd(gpointer addr, guint netmask);


#endif  /* IPSET_INTERNAL_H */
