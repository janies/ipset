/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdbool.h>

#include <bdd.h>
#include <ipset/ipset.h>

bool
IPSET_FUNC(is_empty)(IP_SET_T *set)
{
    /*
     * Since BDDs are unique, the only empty set is the “false” BDD.
     */

    return (set->set_bdd == bddfalse);
}
