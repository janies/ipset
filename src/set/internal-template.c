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

#include <cudd.h>
#include <ipset/ipset.h>
#include <ipset/internal.h>


DdNode *IPSET_NAME(bdd_var)[IP_BIT_SIZE];


void
IPSET_NAME(init_bdd_vars)()
{
    unsigned int  i;

    for (i = 0; i < IP_BIT_SIZE; i++)
    {
        IPSET_NAME(bdd_var)[i] = Cudd_bddNewVar(ipset_manager);
    }
}


DdNode *
IPSET_NAME(make_ip_bdd)(void *vaddr, int netmask)
{
    uint8_t  *addr = (uint8_t *) vaddr;
    DdNode  *result = Cudd_ReadOne(ipset_manager);
    int  i;

    Cudd_Ref(result);
    for (i = 0; i < netmask; i++)
    {
        int  byte_index = i / 8;
        int  bit_number = i % 8;
        int  bit_mask = 0x80 >> bit_number;
        DdNode  *this_bit;
        DdNode  *next;

        this_bit = IPSET_NAME(bdd_var)[i];

        if ((addr[byte_index] & bit_mask) == 0)
        {
            this_bit = Cudd_Not(this_bit);
        }

        next = Cudd_bddAnd(ipset_manager, result, this_bit);
        Cudd_Ref(next);

        Cudd_RecursiveDeref(ipset_manager, result);
        result = next;
    }

    return result;
}
