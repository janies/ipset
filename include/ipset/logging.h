/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#ifndef IPSET_LOGGING_H
#define IPSET_LOGGING_H

#include <glib.h>

#ifdef NDEBUG
#define g_d_debug(...) /* ignore */
#else
#define g_d_debug(...) g_debug(__VA_ARGS__)
#endif

#endif  /* IPSET_LOGGING_H */
