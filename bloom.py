#!/usr/bin/env python
import tornado.ioloop
import tornado.web
from bitarray import bitarray
import hashlib
import sys
import os
import signal
import logging

# Constants

hashpart = 30
m = 2 ** hashpart   # Bloom m-parameter
k = 7       # Bloom k-parameter
listen_port = 8888
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

def getHashes(element):
    H = hashlib.sha224()
    H.update(element)

    h = bitarray()
    h.frombytes(H.digest())

    return [ reduce(lambda A, v: A*2+1 if v else A*2, h[i*hashpart:(i+1)*hashpart], 0) for i in xrange(k) ]

class CmdAddHandler(tornado.web.RequestHandler):
    def get(self):
        element = self.get_argument("e", strip=False)
        hashes = getHashes(element)

        for i in hashes:
            Bloom[i] = True

        self.set_header('Content-Type', 'text/plain; charset="utf-8"')
        self.write("ADDED\n")

class CmdCheckHandler(tornado.web.RequestHandler):
    def get(self):
        element = self.get_argument("e", strip=False)
        hashes = getHashes(element)
        
        self.set_header('Content-Type', 'text/plain; charset="utf-8"')
        for i in hashes:
            if not Bloom[i]:
                self.write("MISSING\n")
                return

        self.write("PRESENT\n")

def term_handler(signum, frame):
    logging.warn("Caught signal %d. Shutting down server...", signum)
    tornado.ioloop.IOLoop.instance().stop()
    logging.warn("Dumping snapshot...")
    with open(snap_path, "wb") as f:
        Bloom.tofile(f)
    logging.warn("Exiting...")
    sys.exit(0)

# Init

Bloom = bitarray()

application = tornado.web.Application([
    (r"/add", CmdAddHandler),
    (r"/check", CmdCheckHandler),
], debug=True)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print >> sys.stderr, "Usage: %s <snapshot_file>" % sys.argv[0]
        sys.exit(1)

    snap_path = sys.argv[1]

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

    signal.signal(signal.SIGTERM, term_handler)
    signal.signal(signal.SIGINT, term_handler)

    application.listen(listen_port)
    tornado.ioloop.IOLoop.instance().start()
