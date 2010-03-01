/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IPSET_INTERNAL_H
#define IPSET_INTERNAL_H

#include <cudd.h>

#include <ipset/ipset.h>

/*
 * The number of bits in an IP address.
 */

#define IPV4_BIT_SIZE  32
#define IPV6_BIT_SIZE  128

/*
 * The CUDD manager for the IP set functions.
 */

extern DdManager *ipset_manager;

/*
 * The BDD variable node for the ith bit of an IP address.  (We count
 * bits starting from the MSB.)
 */

extern DdNode *ipset_ipv4_var[IPV4_BIT_SIZE];
extern DdNode *ipset_ipv6_var[IPV6_BIT_SIZE];

/*
 * Allocate a BDD variable for each bit of an IP address, initializing
 * the ipset_ipvX_var variable.
 */

void
ipset_ipv4_init_vars();

void
ipset_ipv6_init_vars();

/*
 * Create a BDD for an IP address or family of IP addresses.  The
 * corresponding BDD will have boolean variable if the corresponding
 * bit is set in the IP address.  The netmask parameter can be used to
 * limit the number of bits to constrain; if this is less than
 * IPVX_BIT_SIZE, then an entire network will be added to the set.
 */

DdNode *
ipset_ipv4_make_ip_bdd(void *addr, int netmask);

DdNode *
ipset_ipv6_make_ip_bdd(void *addr, int netmask);

#endif  /* IPSET_INTERNAL_H */
