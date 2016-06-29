#!/bin/bash

>&2 echo "Warning! Fill test with large number of items may take many hours."
if [[ $# != 3 ]] ; then
    >&2 printf "Usage: %s <bloom address> <bloom port> <items count>\n" \
        "$0"
    exit 1
fi

HOST="$1"
PORT="$2"
ITEMS="$3"

PY="$(which pypy)"
if [[ $? != 0 ]]; then
    PY="$(which python)"
fi

printf "HOST=%s PORT=%d ITEMS=%d PYTHON=%s\n" "$HOST" "$PORT" "$ITEMS" "$PY"
if [[ $? != 0 ]] ; then
    >&2 echo "Incorrect arguments detected"
    exit 2
fi

{ "$PY" ./fill_bloom.py "$HOST" "$PORT" "$ITEMS" && \
"$PY" ./check_filled.py "$HOST" "$PORT" "$ITEMS" && \
>&2 echo "Fill test: OK" ; } || \
{ >&2 echo "Fill test: FAILED" ; \
exit 1 ; }
