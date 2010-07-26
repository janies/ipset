/* -*- coding: utf-8 -*-
 * ----------------------------------------------------------------------
 * Copyright © 2009-2010, RedJack, LLC.
 * All rights reserved.
 *
 * Please see the LICENSE.txt file in this distribution for license
 * details.
 * ----------------------------------------------------------------------
 */

#include <arpa/inet.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>

#include <ipset/ip.h>
#include "hash.c.in"


void
ipset_ip_from_ipv4(ipset_ip_t *addr, void *v_ipv4)
{
    guint32  *ipv4 = (guint32 *) v_ipv4;

    addr->addr[0] = *ipv4;
    addr->addr[1] = 0;
    addr->addr[2] = 0;
    addr->addr[3] = 0;
    addr->is_ipv4 = TRUE;
}


void
ipset_ip_from_ipv6(ipset_ip_t *addr, void *v_ipv6)
{
    memcpy(addr->addr, v_ipv6, sizeof(guint32) * 4);
    addr->is_ipv4 = FALSE;
}


void
ipset_ip_copy(ipset_ip_t *addr, const ipset_ip_t *src)
{
    memcpy(addr, src, sizeof(ipset_ip_t));
}


guint
ipset_ip_hash(ipset_ip_t *addr)
{
    guint  hash = 0;
    combine_hash(&hash, addr->is_ipv4);
    combine_hash(&hash, addr->addr[0]);
    combine_hash(&hash, addr->addr[1]);
    combine_hash(&hash, addr->addr[2]);
    combine_hash(&hash, addr->addr[3]);
    return hash;
}


gboolean
ipset_ip_equal(const ipset_ip_t *addr1,
               const ipset_ip_t *addr2)
{
    if (addr1 == addr2)
        return TRUE;

    return
        (addr1->is_ipv4 == addr2->is_ipv4) &&
        (memcmp(addr1->addr, addr2->addr, sizeof(guint32) * 4) == 0);
}


gboolean
ipset_ip_from_string(ipset_ip_t *addr, const gchar *str)
{
    int  rc;

    /*
     * Try to parse the line as an IPv4 address.  If that works,
     * return.
     */

    rc = inet_pton(AF_INET, str, addr->addr);
    if (rc == 1)
    {
        addr->addr[1] = 0;
        addr->addr[2] = 0;
        addr->addr[3] = 0;
        addr->is_ipv4 = TRUE;
        return TRUE;
    }

    /*
     * If that didn't work, try IPv6.
     */

    rc = inet_pton(AF_INET6, str, addr->addr);
    if (rc == 1)
    {
        addr->is_ipv4 = FALSE;
        return TRUE;
    }

    /*
     * Otherwise, we've got an error.
     */

    return FALSE;
}


#define IPV4_BUFFER_LENGTH  (sizeof "xxx.xxx.xxx.xxx")
static gchar IPV4_BUFFER[IPV4_BUFFER_LENGTH];


static void
raw_ipv4_to_string(gchar *dest, const guint8 *bytes)
{
    g_snprintf(dest, IPV4_BUFFER_LENGTH,
               "%u.%u.%u.%u",
               bytes[0], bytes[1], bytes[2], bytes[3]);
}


static const gchar *
ipv4_to_string(const ipset_ip_t *addr)
{
    raw_ipv4_to_string(IPV4_BUFFER,
                       (const guint8 *) addr->addr);
    return IPV4_BUFFER;
}


#define IPV6_BUFFER_LENGTH \
    (sizeof "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255")
static gchar IPV6_BUFFER[IPV6_BUFFER_LENGTH];

#define NS_IN6ADDRSZ 16
#define NS_INT16SZ 2


static const gchar *
ipv6_to_string(const ipset_ip_t *addr)
{
    const guint8  *src = (const guint8 *) addr->addr;

    /*
     * Note that int32_t and int16_t need only be "at least" large enough
     * to contain a value of the specified size.  On some systems, like
     * Crays, there is no such thing as an integer variable with 16 bits.
     * Keep this in mind if you think this function should have been coded
     * to use pointer overlays.  All the world's not a VAX.
     */
    gchar *tp;
    struct { gint base, len; } best, cur;
    guint words[NS_IN6ADDRSZ / NS_INT16SZ];
    int i;

    /*
     * Preprocess:
     *      Copy the input (bytewise) array into a wordwise array.
     *      Find the longest run of 0x00's in src[] for :: shorthanding.
     */
    memset(words, '\0', sizeof words);
    for (i = 0; i < NS_IN6ADDRSZ; i++)
        words[i / 2] |= (src[i] << ((1 - (i % 2)) << 3));
    best.base = -1;
    best.len = 0;
    cur.base = -1;
    cur.len = 0;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        if (words[i] == 0) {
            if (cur.base == -1)
                cur.base = i, cur.len = 1;
            else
                cur.len++;
        } else {
            if (cur.base != -1) {
                if (best.base == -1 || cur.len > best.len)
                    best = cur;
                cur.base = -1;
            }
        }
    }
    if (cur.base != -1) {
        if (best.base == -1 || cur.len > best.len)
            best = cur;
    }
    if (best.base != -1 && best.len < 2)
        best.base = -1;

    /*
     * Format the result.
     */
    tp = IPV6_BUFFER;
    for (i = 0; i < (NS_IN6ADDRSZ / NS_INT16SZ); i++) {
        /* Are we inside the best run of 0x00's? */
        if (best.base != -1 && i >= best.base &&
            i < (best.base + best.len)) {
            if (i == best.base)
                *tp++ = ':';
            continue;
        }
        /* Are we following an initial run of 0x00s or any real hex? */
        if (i != 0)
            *tp++ = ':';
        /* Is this address an encapsulated IPv4? */
        if (i == 6 && best.base == 0 &&
            (best.len == 6 || (best.len == 5 && words[5] == 0xffff))) {
            raw_ipv4_to_string(tp, src+12);
            tp += strlen(tp);
            break;
        }
        tp += g_sprintf(tp, "%x", words[i]);
    }
    /* Was it a trailing run of 0x00's? */
    if (best.base != -1 && (best.base + best.len) ==
        (NS_IN6ADDRSZ / NS_INT16SZ))
        *tp++ = ':';
    *tp++ = '\0';

    /*
     * And we're done.
     */

    return IPV6_BUFFER;
}


const gchar *
ipset_ip_to_string(const ipset_ip_t *addr)
{
    if (addr->is_ipv4)
    {
        return ipv4_to_string(addr);
    } else {
        return ipv6_to_string(addr);
    }
}
