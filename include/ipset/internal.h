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
 * The ADD variable node for the ith bit of an IP address.  (We count
 * bits starting from the MSB.)
 */

extern DdNode *ipset_ipv4_add_var[IPV4_BIT_SIZE];
extern DdNode *ipset_ipv6_add_var[IPV6_BIT_SIZE];


/*
 * An ADD node representing a positive assignment for the ith bit of
 * an IP address.  Evaluates to 1 when the bit is set, and to 0 when
 * it isn't.
 */

extern DdNode *ipset_ipv4_add_var_on[IPV4_BIT_SIZE];
extern DdNode *ipset_ipv6_add_var_on[IPV6_BIT_SIZE];


/*
 * An ADD node representing a negative assignment for the ith bit of
 * an IP address.  Evaluates to 0 when the bit is set, and to 1 when
 * it isn't.
 */

extern DdNode *ipset_ipv4_add_var_off[IPV4_BIT_SIZE];
extern DdNode *ipset_ipv6_add_var_off[IPV6_BIT_SIZE];


/*
 * The BDD variable node for the ith bit of an IP address.  (We count
 * bits starting from the MSB.)
 */

extern DdNode *ipset_ipv4_bdd_var[IPV4_BIT_SIZE];
extern DdNode *ipset_ipv6_bdd_var[IPV6_BIT_SIZE];


/*
 * Allocate an ADD variable for each bit of an IP address,
 * initializing the ipset_ipvX_add_var variable.
 */

void
ipmap_ipv4_init_add_vars();

void
ipmap_ipv6_init_add_vars();


/*
 * Allocate a BDD variable for each bit of an IP address, initializing
 * the ipset_ipvX_bdd_var variable.
 */

void
ipset_ipv4_init_bdd_vars();

void
ipset_ipv6_init_bdd_vars();


/*
 * Create an ADD for an IP address or family of IP addresses.  The
 * corresponding BDD will have each boolean variable set if the
 * corresponding bit is set in the IP address.  The netmask parameter
 * can be used to limit the number of bits to constrain; if this is
 * less than IPVX_BIT_SIZE, then an entire network will be added to
 * the set.  The values of the ADD will all be 0 or 1, so the ADD is
 * acceptable to pass in as the condition in a call to Cudd_addIte and
 * friends.
 */

DdNode *
ipmap_ipv4_make_ip_add(void *addr, int netmask);

DdNode *
ipmap_ipv6_make_ip_add(void *addr, int netmask);


/*
 * Create a BDD for an IP address or family of IP addresses.  The
 * corresponding BDD will have each boolean variable set if the
 * corresponding bit is set in the IP address.  The netmask parameter
 * can be used to limit the number of bits to constrain; if this is
 * less than IPVX_BIT_SIZE, then an entire network will be added to
 * the set.
 */

DdNode *
ipset_ipv4_make_ip_bdd(void *addr, int netmask);

DdNode *
ipset_ipv6_make_ip_bdd(void *addr, int netmask);


#endif  /* IPSET_INTERNAL_H */
