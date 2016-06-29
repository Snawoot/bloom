#!/usr/bin/python
import sys
import urllib
import urllib2
import random
import json

rnd_len_sigma = 1000**0.5
rnd_len_min = 10

def rnd_str():
    length = int(rnd_len_min+abs(random.gauss(0,rnd_len_sigma)))
    return "".join(
        chr(random.randint(1,255)) for _ in xrange(length)
    )

if __name__ == '__main__':
    if len(sys.argv) != 4:
        print >> sys.stderr, (
            "Usage: %s <bloom address> <bloom port> <items count>"
            ) % sys.argv[0]
        exit(1)
    address, port, count = sys.argv[1:4]
    port = int(port)
    count = int(count)

    requrl = 'http://%s:%d/check?e=' % (address, port)
    present = 0
    for i in xrange(count):
        w = rnd_str()
        q = urllib.quote(w)
        ans = urllib2.urlopen(requrl + q, timeout = 0.5).read()
        if ans == 'PRESENT\n':
            present += 1
            print "Collision", q
        elif ans == 'MISSING\n':
            pass
        else:
            assert False

    cr = float(present) / count
    res = {
        'count': count,
        'collisions': present,
        'coll_rate': cr,
        'coll_rate_pretty': "{0:.4f}%".format(cr * 100)
    }
    json.dump(res, sys.stdout, indent=4, sort_keys = True)
