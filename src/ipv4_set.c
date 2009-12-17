/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */


/*
 * The IPv4 and IPv6 set types are basically identical, except for the
 * names of the functions, and the size of the values that are being
 * stored.  Rather than having two mostly duplicate definitions of
 * each function, we define “template functions” where anything that
 * depends on the size of the IP address is defined using the
 * following macros.
 */


/**
 * The name of the ipvX_set_t type.
 */

#define IP_SET_T ipv4_set_t

/**
 * Creates a function name of the form “ipset_ipv4_<basename>”.
 */

#define IPSET_FUNC(basename) ipset_ipv4_##basename


/*
 * Now include all of the templates.
 */

#include "allocation-template.c"
#include "inspection-template.c"
