#!/usr/bin/python

# Borrowed from http://hur.st/bloomfilter

import sys
from math import log, pow, ceil

help_tpl = (
        "Usage:     %(progname)s <number of elements> <desired FP probability>\n"
        "\n"
        "Example:   %(progname)s 500000000 0.001\n"
        "\n"
        "This command will calculate size in bits (m) and number of keys (k) "
        "for bloom filter, capable containing 500,000,000 elements with "
        "probability of false positives 0.1%%"
        )

def usage():
    print >> sys.stderr, help_tpl % {'progname': sys.argv[0]}
    exit(1)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        usage()
    n, p = map(float, sys.argv[1:3])
    m = ceil((n * log(p)) / log(1.0 / (pow(2.0, log(2.0)))))
    m2 = int(2**ceil(log(m,2)))
    k = round(log(2.0) * m / n)
    k2 = round(log(2.0) * m2 / n)
    print "m=%d k=%d m_padded=%d k_padded=%d" % (m, k, m2, k2)
