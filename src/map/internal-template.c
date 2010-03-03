/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
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


DdNode *IPMAP_NAME(add_var)[IP_BIT_SIZE];
DdNode *IPMAP_NAME(add_var_on)[IP_BIT_SIZE];
DdNode *IPMAP_NAME(add_var_off)[IP_BIT_SIZE];


void
IPMAP_NAME(init_add_vars)()
{
    DdNode  *one;
    DdNode  *zero;
    unsigned int  i;

    one = Cudd_ReadOne(ipset_manager);
    zero = Cudd_ReadZero(ipset_manager);

    for (i = 0; i < IP_BIT_SIZE; i++)
    {
        /*
         * Unlike BDD variables, ADD variables need to have their
         * reference count incremented.
         */

        IPMAP_NAME(add_var)[i] = Cudd_addNewVar(ipset_manager);
        Cudd_Ref(IPMAP_NAME(add_var)[i]);

        /*
         * Create the positive assignment for this variable.
         */

        IPMAP_NAME(add_var_on)[i] =
            Cudd_addIte(ipset_manager, IPMAP_NAME(add_var)[i],
                        one, zero);
        Cudd_Ref(IPMAP_NAME(add_var_on)[i]);

        /*
         * Create the negative assignment for this variable.
         */

        IPMAP_NAME(add_var_off)[i] =
            Cudd_addIte(ipset_manager, IPMAP_NAME(add_var)[i],
                        zero, one);
        Cudd_Ref(IPMAP_NAME(add_var_off)[i]);
    }
}


DdNode *
IPMAP_NAME(make_ip_add)(void *vaddr, int netmask)
{
    uint8_t  *addr = (uint8_t *) vaddr;
    DdNode  *result = Cudd_ReadOne(ipset_manager);
    int  i;

    /*
     * Special case — the ADD for a netmask that's out of range never
     * evaluates to true.
     */

    if ((netmask <= 0) || (netmask > IP_BIT_SIZE))
    {
        return Cudd_ReadZero(ipset_manager);
    }

    Cudd_Ref(result);
    for (i = 0; i < netmask; i++)
    {
        int  byte_index = i / 8;
        int  bit_number = i % 8;
        int  bit_mask = 0x80 >> bit_number;
        DdNode  *this_bit;
        DdNode  *next;


        if ((addr[byte_index] & bit_mask) == 0)
        {
            this_bit = IPMAP_NAME(add_var_off)[i];
            Cudd_Ref(this_bit);
        } else {
            this_bit = IPMAP_NAME(add_var_on)[i];
            Cudd_Ref(this_bit);
        }

        next = Cudd_addApply(ipset_manager, Cudd_addTimes,
                             result, this_bit);
        Cudd_Ref(next);

        Cudd_RecursiveDeref(ipset_manager, this_bit);
        Cudd_RecursiveDeref(ipset_manager, result);
        result = next;
    }

    return result;
}
