#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/queue.h>
#include <evhttp.h>
#include <limits.h>
#include <openssl/sha.h>

// Bloom-filter parameters
#define hashpart 33
#define m ( 1L << hashpart )
#define k 10

#define BIND_ADDRESS "0.0.0.0"
#define BIND_PORT 8888

#define MIME_TYPE "Content-Type", "text/html; charset=UTF-8"
#define MEGA (1<<20)
#define STR_MAX 1024
#define DEBUGHASH
#define BITS_PER_CELL (sizeof(bloom_cell) * CHAR_BIT)

typedef unsigned long bloom_cell;
bloom_cell *Bloom = NULL;
unsigned char hashbuf[SHA384_DIGEST_LENGTH];
bloom_cell Ki[k];

#ifdef DEBUGHASH
char hexdigits[] = "0123456789abcdef";
#endif

const unsigned char *Hash(const char* bytes)
{
    SHA384(bytes,  strnlen(bytes, STR_MAX), hashbuf);
    #ifdef DEBUGHASH
    char hexdigest[SHA384_DIGEST_LENGTH * 2 + 1];
    int i;
    for (i=0; i < SHA384_DIGEST_LENGTH; i++) {
        hexdigest[2*i]   = hexdigits[hashbuf[i] >> (CHAR_BIT / 2)];
        hexdigest[2*i+1] = hexdigits[hashbuf[i] & 0xF ];
    }
    hexdigest[sizeof(hexdigest)-1]='\0';
    fprintf(stderr, "%s\n", hexdigest);
    #endif

    int i=0, j=0;
    bool bit;
    for (; i < k; i++) {
        for (; ; j++) {
            bit = (hashbuf[j / CHAR_BIT]
        }
    }
    return hashbuf;
}

bool GetBit(bloom_cell *bv, size_t n)
{
    return (bv[n / BITS_PER_CELL] & ((bloom_cell)1 << (n % BITS_PER_CELL))) != 0;
}

void JumpBit(bloom_cell *bv, size_t n)
{
    bv[n / (sizeof(bloom_cell) * CHAR_BIT)] |= ((bloom_cell)1 << ( n % (sizeof(bloom_cell) * CHAR_BIT) ) );
}

int main()
{
    fprintf(stderr, "Allocating arena with size %.2f MBytes ...\n", (float)m / CHAR_BIT / MEGA);
    Bloom = malloc( (m + ( CHAR_BIT - 1)) / CHAR_BIT ); // Ceil byte length: bytes = bits + 7 / 8

    if (!event_init())
    {
        fputs("Failed to init libevent.", stderr);
        return -1;
    }
    struct evhttp *Server = evhttp_start(BIND_ADDRESS, BIND_PORT);
    if (!Server)
    {
        fputs("Failed to init HTTP-server.", stderr);
        return -1;
    }

    void OnReq(struct evhttp_request *req, void *arg)
    {
        struct evbuffer *OutBuf = evhttp_request_get_output_buffer(req);
        if (!OutBuf) {
            evhttp_send_reply(req, HTTP_BADREQUEST, "Bad Request", OutBuf);
            return;
        }
        struct evkeyvalq *Headers = evhttp_request_get_output_headers(req);
        if (!Headers) {
            evhttp_send_reply(req, HTTP_INTERNAL, "Internal Error", OutBuf);
            return;
        }
        const struct evhttp_uri *HTTPURI =  evhttp_request_get_evhttp_uri(req);
        if (!HTTPURI) {
            evhttp_send_reply(req, HTTP_BADREQUEST, "Bad Request", OutBuf);
            return;
        }
        const char *query_string = evhttp_uri_get_query(HTTPURI);
        if (!query_string) {
            evhttp_send_reply(req, HTTP_BADREQUEST, "Element Required", OutBuf);
            return;
        }
        struct evkeyvalq params;
        evhttp_parse_query_str(query_string, &params);
        const char *element = evhttp_find_header(&params, "e");
        if (!element) {
            evhttp_clear_headers(&params);
            evhttp_send_reply(req, HTTP_BADREQUEST, "Element Required", OutBuf);
            return;
        }

        Hash(element);
        evhttp_add_header(Headers, MIME_TYPE);
        evbuffer_add_printf(OutBuf, "Response: %s\n", element);
        evhttp_send_reply(req, HTTP_OK, "OK", OutBuf);
        evhttp_clear_headers(&params);
    };
    evhttp_set_gencb(Server, OnReq, 0);

    if (event_dispatch() == -1)
    {
        fputs("Failed to run message loop.", stderr);
        return -1;
    }

    evhttp_free(Server);
    free(Bloom);
    return 0;
}
