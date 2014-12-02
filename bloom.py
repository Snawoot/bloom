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

hashpart = 33
m = 2 ** hashpart   # Bloom m-parameter
k = 10       # Bloom k-parameter
listen_port = 8888
logging.basicConfig(level=logging.DEBUG, format='%(asctime)s - %(levelname)s - %(message)s')

response_headers = ('Content-Type', 'text/plain; charset="utf-8"')
miss_response  = "MISSING\n"
hit_response   = "PRESENT\n"
added_response = "ADDED\n"

def getHashes(element):
    H = hashlib.sha384()
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

        self.set_header(*response_headers)
        self.write(added_response)

class CmdCheckHandler(tornado.web.RequestHandler):
    def get(self):
        element = self.get_argument("e", strip=False)
        hashes = getHashes(element)
        
        self.set_header(*response_headers)
        for i in hashes:
            if not Bloom[i]:
                self.write(miss_response)
                return

        self.write(hit_response)

class CmdCheckThenAddHandler(tornado.web.RequestHandler):
    def get(self):
        element = self.get_argument("e", strip=False)
        hashes = getHashes(element)

        present = True
        for i in hashes:
            if not Bloom[i]:
                present = False
                break

        if not present:
            for i in hashes:
                Bloom[i] = True

        self.set_header(*response_headers)
        self.write(hit_response if present else miss_response)

def term_handler(signum, frame):
    logging.warn("Caught signal %d. Shutting down server...", signum)
    tornado.ioloop.IOLoop.instance().stop()
    logging.warn("Dumping snapshot...")
    with open(snap_path, "wb") as f:
        Bloom.tofile(f)
    logging.warn("Exiting...")
    sys.exit(0)

def dump_handler(signum, frame):
    if not dump_children:
        logging.warn("Caught signal %d. Doing fork and dump...", signum)
    else:
        logging.error("Dumper process already running!")
        return

    try:
        pid = os.fork()
    except OSError:
        logging.error("Unable to fork!")
        return

    if pid == 0:
        logging.warn("Dumping snapshot...")
        try:
            with open(snap_path, "wb") as f:
                Bloom.tofile(f)
        except:
            logging.error("Snapshot dump failed.")
            sys.exit(10)
        logging.warn("Child exiting.")
        sys.exit(0)
    else:
        dump_children.append(pid)

def child_collector(signum, frame):
    for cpid in dump_children:
        try:
            returned_pid, status = os.waitpid(cpid, os.WNOHANG)
        except OSError:
            logging.warn("Child with pid=%d already dead.", cpid)
            dump_children.remove(cpid)
            continue
        if cpid == returned_pid:
            logging.warn("Child with pid=%d has finished with exitcode %d. Collected him.", cpid, status / 0x100)
            dump_children.remove(cpid)

# Init

Bloom = bitarray()

application = tornado.web.Application([
    (r"/add", CmdAddHandler),
    (r"/check", CmdCheckHandler),
    (r"/checkthenadd", CmdCheckThenAddHandler),
], debug=True)

dump_children = []

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
    signal.signal(signal.SIGCHLD, child_collector)
    signal.signal(signal.SIGUSR1, dump_handler)

    application.listen(listen_port)
    tornado.ioloop.IOLoop.instance().start()
