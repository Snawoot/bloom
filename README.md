Bloom [![Build Status](https://travis-ci.org/Snawoot/bloom.svg?branch=master)](https://travis-ci.org/Snawoot/bloom)
=====

Bloom is a server, which contains [Bloom filter probabilistic data structure](https://en.wikipedia.org/wiki/Bloom_filter) in memory, provides access to it via HTTP and ensures data persistence on disk by mean of atomic consistent snapshots.

## Building

See also [Prebuilt Docker image](#prebuilt-docker-image) and binaries on Releases page to use prebuilt ones.

#### Debian/Ubuntu

Run these commands in sources directory:

```bash
sudo apt-get install build-essential libevent-dev
make
```

#### RHEL/OEL/CentOS

Run these commands in sources directory:

```bash
sudo yum install gcc libevent2-devel make
make
```

Run `make static` instead of `make` to build static binary.

#### Mac OS X

Assuming you are using [Homebrew](http://brew.sh/)

```bash
brew install libevent
make
```

Static build for Mac OS X is not available now.

#### FreeBSD

According to `siege` benchmarks, GCC compiler gains better performance for this application. If you want to use BSD `cc`, you may change CC variable in Makefile. Application can be built using both of them.

```bash
pkg install gcc libevent2
make
```

Run `make static` instead of `make` to build static binary.

#### Solaris

You have to build libevent2 before:

```bash
sudo pkg install gcc
wget https://github.com/libevent/libevent/releases/download/release-2.0.22-stable/libevent-2.0.22-stable.tar.gz
tar xf libevent-2.0.22-stable.tar.gz
cd libevent-2.0.22-stable
./configure
make
sudo make install
```
You may also need to add `/usr/local/lib` to library search path:

```bash
sudo crle
# Settings output here. Check output and add /usr/local/lib at the end, delimiting it by colon
sudo crle -l /lib:/usr/lib:/usr/local/lib
```

After that, run build of Bloom from its directory:

```bash
make
```

Static build for Solaris is not available now.

## Installing

### Prebuilt Docker image

Run:

```bash
docker volume bloom
docker run -dit \
    -v bloom:/var/lib/bloom \
    -p 8889:8889 \
    --restart unless-stopped \
    --name bloom \
    yarmak/bloom \
    /var/lib/bloom/bloom.dat
```

Help:

```bash
docker run -it \
    yarmak/bloom \
    -h
```
### From source built before

```bash
make install
```
to install dynamic binary.

## Usage

### Running daemon

`bloom <filename_for_snapshot>` or
`./bloom.static <filename_for_snapshot>` if you prefer statically linked version. 

Command line options:

```
$ bloom -h
Usage: bloom [options] SNAPSHOT_FILE

Options:

-H BIND_ADDRESS		HTTP interface bind address. Default: 0.0.0.0

-P BIND_PORT		HTTP interface bind port. Default: 8889

-h			Print this help message

-m M			Number of bits in bloom filter. Default: 2^33

-k K			Number of hash functions. Default: 10

-t SECONDS		Dump bloom filter snapshot to file every SECONDS
			seconds. You can set this value to 0 if you wish
			to disable this feature - snapshots are taken on USR1
			signal and at exit in any case.
```

Default settings is suitable for containing 500,000,000 elements with false positive probability 0.1%. See also [Utilities](https://github.com/Snawoot/bloom#utilities) for parameters calculator.

### Querying

Test whether an element is a member of a set:
```
$ curl http://127.0.0.1:8889/check?e=sdfdsafdsafsadf
MISSING
```
Add an element to set:
```
$ curl http://127.0.0.1:8889/add?e=sdfdsafdsafsadf
ADDED
```
Check then add at once:
```
$ curl http://127.0.0.1:8889/checkthenadd?e=aaaaaabbb
MISSING
$ curl http://127.0.0.1:8889/checkthenadd?e=aaaaaabbb
PRESENT
```
### Saving set

Server saves data to snapshot file in following cases:
* Server exit (received `SIGTERM` or `SIGINT`)
* Timer event. By default server dumps snapshot to disk every 5 minutes. See also help for option `-t`.
* On `SIGUSR1` signal. This way you may control dump process on your own by sending signal to daemon.

Snapshot dumping process does not blocks serving request and uses copy-on-write method, so dumped data is always consistent.

## Utilities

* `utils/collision_meter.py` - Check structure occupancy by measuring false positive probability on completely random requests.
* `utils/bf_calc.py` - Calculate parameters of bloom filter for given number of elements and false positives probability.
