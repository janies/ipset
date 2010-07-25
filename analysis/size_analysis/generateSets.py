# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2010, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

import random
import sys
import getopt

def process_args():
    size = None
    length = None
    version = 4
    try:
        opts, args = getopt.getopt(sys.argv[1:],
                                   "s:l:v:h",
                                   ["size=","length=",
                                    "version=", "help"])
    except getopt.GetoptError, err:
        error(str(err))
    for o,a in opts:
        if o in ("-h", "--help"):
            usage()
        elif o in ("-s", "--size"):
            size = int(a)
        elif o in ("-l", "--length"):
            length = int(a)
        elif o in ("-v", "--version"):
            version = int(a)

    if version != 4 and version !=6:
        error("Version number must be 4 or 6")
    if length is None or size is None:
        error("Length and size must be specified")
    if version == 4 and (length < 0 or length > 32):
        error("Length must be a value between 0 and 32 for IPv4 addresses")
    if version == 6 and (length < 0 or length > 128):
        error("Length must be a value between 0 and 128 for IPv6 addresses")
    if size < 1:
        error("Size must be an integer value greater than 0")
    if length < 0:
        error("Length must be 0 or greater")
    return version, size, length

def error(s):
    sys.stderr.write("ERROR: %s\n\n" % s)
    usage()

def usage():
    sys.stderr.write(("USAGE: %s {-s|--size=SAMPLE_SIZE} {-l |" +
                      "--length=BIT_LENGTH}\n       " +
                      "[-v|--version=VERSION] [-h|--help]\n\n" +
                      "\t %s ARG=SAMPLE_SIZE: SAMPLE_SIZE is " +
                      " the number of IP addresses produced\n" + 
                      "\t %s ARG=BIT_LENGTH: BIT_LENGTH is " +
                      "the bit mask used.\n" +
                      "\t %s ARG=VERION:  The either 4 or 6 for " +
                      "IP version number\n\n") % (sys.argv[0],
                      "-s|--size", "-l|--length", "-v|--version"))
    sys.exit(0)

def mask_bits(length, version):
    if version == 4:
        val = 0
        count = 32 - length
        while count < 32:
            #print count
            val += pow(2, count)
            count += 1
    elif version == 6:
        val = []
        for s in range(16):
            tmp = 0
            if length >= 8:
                shift = 8
            else:
                shift = length
            length -= shift
            tmp = tmp << shift
            for s in range(shift):
                tmp = (tmp >> 1) + pow(2, 7)
            val.append(tmp)
        
    else:
        val = None
    return val

def print_ips(num, version):
    if version == 4:
        d = num & 0xff
        num = num >> 8
        c = num & 0xff
        num = num >> 8
        b = num & 0xff
        num = num >> 8
        a = num & 0xff
        return "%d.%d.%d.%d" % (a, b, c, d)
    if version == 6:
        if not isinstance(num, list) or len(num) != 16:
            return "0000:::::::"
        s = ""
        for a,b in ((0,1),(2,3),(4,5),(6,7),(8,9),(10,11),(12,13),(14,15)):
            s += "%02x%02x:" % (num[a], num[b])
        s = s[:-1]
        return s
def gen_numbers(version, size, length):
    low = mask_bits(length, version)

    numList = []

    if version == 6:
        high = mask_bits(128, version)
    else:
        high = mask_bits(32, version) 
    sys.stderr.write("Generating addresses in the range \n%s - %s\n" %
           (print_ips(low,version), print_ips(high,version)))
    for s in range(size):
        numList.append(gen_random_number(version, low, high))
    return numList

def gen_random_number(version, low, high):
    if version == 4:
        return random.randint(low,high)
    elif version == 6:
        val = []
        for a in range(16):
            val.append(random.randint(low[a], high[a]))
        return val

def main():
    version, size, length = process_args()
    sys.stderr.write("version = %d\nsize = %d\nlength = %d" %
                     (version, size, length))
    numList = gen_numbers(version, size, length)
    for num in numList:
        print print_ips(num, version)

if __name__ == "__main__":
    main()
