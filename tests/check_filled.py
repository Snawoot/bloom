#!/usr/bin/python
import sys
import urllib
import urllib2
import inflect

if __name__ == '__main__':
    if not (4 <= len(sys.argv) <= 5):
        print >> sys.stderr, (
            "Usage: %s <bloom address> <bloom port> < [range start] <range end> >"
            ) % sys.argv[0]
        exit(1)
    address, port = sys.argv[1:3]
    port = int(port)
    rng = map(int, sys.argv[3:5])

    requrl = 'http://%s:%d/check?e=' % (address, port)
    p = inflect.engine()
    for i in xrange(*rng):
        w = p.number_to_words(i)
        q = urllib.quote(w)
        #print requrl + q
        ans = urllib2.urlopen(requrl + q, timeout = 0.5).read()
        assert ans == 'PRESENT\n'
