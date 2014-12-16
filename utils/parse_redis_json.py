#!/usr/bin/env pypy
import sys
import json
import pickle

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print >> sys.stderr, "Usage: %s <rdb_json_dump>" % sys.argv[0]
        sys.exit(1)

    json_path = sys.argv[1]
    with open(json_path) as jf:
        J = json.load(jf)
    for session_key, session_dict in J[0].iteritems():
        for video in iter(pickle.loads(session_dict["data"])["vidos-seen"]):
            sid = session_key.partition("::")[2]
            print "|".join((video, sid, "1_first"))
            print "|".join((video, sid, "1_second"))
#>>>> pickle.loads(J[0][u'sid::0:95495529b19f484f9f2fe60cd7b7bfd2']['data'])
#   {'vidos-seen': {'5050523|0': u'9ace13_2629122427'}}
