bloom
=====

The `bloom` is an in-memory persistent storage for determining whether an element is member of set (useful for counting uniqie events, sessions, storing approximated sets etc). Uses a Bloom filter - a space-efficient probabilistic data structure.
Default configuration is sufficient for testing the set of 500,000,000 elements with false-positive probability 0.1%.

### Installation
`sudo apt-get install python-bitarray python-gevent`
or use `pip` to install corresponding modules.

### Running
`python bloom.py <filename_for_snapshot>`

### Usage
Test whether an element is a member of a set:
```
$ curl localhost:8888/check?e=sdfdsafdsafsadf
MISSING
```
Add an element to set:
```
$ curl localhost:8888/add?e=sdfdsafdsafsadf
ADDED
```
Check and add at a time:
```
$ curl localhost:8888/checkthenadd?e=sdfdsafdsafsadf
PRESENT
```
### Saving set
Server saves data to snapshot file on server exit.
In addition, you may force the server to dump snapshot on disk by sending USR1 signal. Snapshot dumping process does not blocks serving request and uses copy-on-write method, so dumped data is always consistent.
