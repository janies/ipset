/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <stdint.h>
#include <stdlib.h>

#include <bdd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


int IPSET_NAME(starting_var);


void
IPSET_NAME(init_vars)()
{
    IPSET_NAME(starting_var) = bdd_extvarnum(IP_BIT_SIZE);
}


BDD
IPSET_NAME(make_ip_bdd)(void *vaddr, int netmask)
{
    uint8_t  *addr = (uint8_t *) vaddr;
    BDD  result = bddtrue;
    int  var;
    int  i;

    for (var = IPSET_NAME(starting_var), i = 0;
         i < netmask;
         var++, i++)
    {
        int  byte_index = i / 8;
        int  bit_number = i % 8;
        int  bit_mask = 0x80 >> bit_number;
        BDD  next;
        BDD  this_bit;

        if ((addr[byte_index] & bit_mask) != 0)
            this_bit = bdd_ithvar(var);
        else
            this_bit = bdd_nithvar(var);

        next = bdd_addref(bdd_and(result, this_bit));
        bdd_delref(result);
        result = next;
    }

    return result;
}
