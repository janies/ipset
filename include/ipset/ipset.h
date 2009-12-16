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


int ipset_init();

#endif  /* IPSET_IPSET_H */
