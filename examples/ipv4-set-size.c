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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <ipset/ipset.h>


typedef uint8_t  ipv4_addr_t[4];


static inline void
random_ip(ipv4_addr_t *ip)
{
    int  i;

    for (i = 0; i < sizeof(ipv4_addr_t); i++)
    {
        uint8_t  random_byte = random() & 0xff;
        (*ip)[i] = random_byte;
    }
}


static void
one_test(long num_elements)
{
    ip_set_t  set;
    long  i;
    size_t  size;
    double  size_per_element;

    ipset_init(&set);

    for (i = 0; i < num_elements; i++)
    {
        ipv4_addr_t  ip;

        random_ip(&ip);
        ipset_ipv4_add(&set, &ip);
    }

    size = ipset_memory_size(&set);
    size_per_element = ((double) size) / num_elements;
    fprintf(stdout, "%lu %zu %.3f\n", num_elements, size, size_per_element);

    ipset_done(&set);
}


int
main(int argc, const char **argv)
{
    long  num_tests;
    long  num_elements;
    long  i;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: ipv4-set-size [# tests] [# elements]\n");
        return -1;
    }

    num_tests = atol(argv[1]);
    num_elements = atol(argv[2]);

    fprintf(stderr, "Creating %lu sets with %lu elements each.\n",
            num_tests, num_elements);

    ipset_init_library();
    srandom(time(NULL));

    for (i = 0; i < num_tests; i++)
    {
        one_test(num_elements);
    }

    return 0;
}
