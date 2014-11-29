bloom
=====

Server for checking web-sessions uniqueness. Technical preview.

### Installation:
`sudo apt-get install python-bitarray python-tornado`

### Examples:
```
$ curl localhost:8888/check?e=sdfdsafdsafsadf
MISSING
$ curl localhost:8888/add?e=sdfdsafdsafsadf
ADDED
$ curl localhost:8888/check?e=sdfdsafdsafsadf
PRESENT
```

