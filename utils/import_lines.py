#!/usr/bin/env python
from bitarray import bitarray
import hashlib
import sys
import os
import logging

# Constants

hashpart = 33
m = 2 ** hashpart   # Bloom m-parameter
k = 10       # Bloom k-parameter
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def getHashes(element):
    H = hashlib.sha384()
    H.update(element)

    h = bitarray()
    h.frombytes(H.digest())

    return [ reduce(lambda A, v: A*2+1 if v else A*2, h[i*hashpart:(i+1)*hashpart], 0) for i in xrange(k) ]

def CmdAddHandler(hashes):
    for i in hashes:
        Bloom[i] = True

# Init

Bloom = bitarray()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print >> sys.stderr, "Usage: %s <snapshot_file> <rdb_json_dump>" % sys.argv[0]
        sys.exit(1)

    snap_path = sys.argv[1]
    text_path = sys.argv[2]

    if os.access(snap_path, os.F_OK):
        logging.info("Loading %.2f MBytes bitvector from file...", (m / float(2**20) / 8) )
        with open(snap_path, "rb") as f:
            Bloom.fromfile(f, (m + 7) / 8)
    else:
        logging.info("Initializing %.2f MBytes bitvector and writing to file ...", (m / float(2**20) / 8) )
        Bloom = bitarray(m)
        Bloom.setall(False)
        with open(snap_path, "wb") as f:
            Bloom.tofile(f)
    logging.info("Initialization complete")

    with open(text_path) as tf:
        for line in tf:
            CmdAddHandler(getHashes( line.rstrip('\n') ))

    logging.warn("Server has been shut down. Dumping snapshot...")
    with open(snap_path, "wb") as f:
        Bloom.tofile(f)
    logging.warn("Exiting...")
    sys.exit(0)
