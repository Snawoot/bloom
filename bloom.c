#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/queue.h>
#include <evhttp.h>
#include <limits.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <signal.h>

// Bloom-filter parameters
#define hashpart 33
#define m ( 1L << hashpart )
#define k 10

// Network parameters
#define BIND_ADDRESS "0.0.0.0"
#define BIND_PORT 8888

// Internal definitions
#define MIME_TYPE "Content-Type", "text/html; charset=UTF-8"
#define MEGA (1<<20)
#define STR_MAX 1024
#define BITS_PER_CELL (sizeof(bloom_cell) * CHAR_BIT)
const char miss_response[]  = "MISSING\n";
const char hit_response[]   = "PRESENT\n";
const char added_response[] = "ADDED\n";

typedef unsigned char bloom_cell;

//Hasher
unsigned char hashbuf[SHA384_DIGEST_LENGTH];
size_t Ki[k];
size_t *Hashes(const char* bytes)
{
    SHA384(bytes,  strnlen(bytes, STR_MAX), hashbuf);

    int bit, i, j, n=0;
    for (i=0; i < k; i++) {
        size_t curr_key=0;
        for (j=0; j<hashpart; j++,n++) {
            bit = (hashbuf[n / CHAR_BIT] & ((unsigned char)1 << ((CHAR_BIT - 1) - (n % CHAR_BIT)))) !=0 ? 1 : 0;
            curr_key = (curr_key << 1) | bit;
        }
        Ki[i] = curr_key;
    }
    return Ki;
}

//Bloom operations
bool GetBit(bloom_cell *bv, size_t n)
{
    return (bv[n / BITS_PER_CELL] & ((bloom_cell)1 << ((BITS_PER_CELL - 1) - (n % BITS_PER_CELL)))) != 0;
}

void JumpBit(bloom_cell *bv, size_t n)
{
    bv[n / BITS_PER_CELL] |= ((bloom_cell)1 << ((BITS_PER_CELL - 1) - (n % BITS_PER_CELL )) );
}

//URI (commands) handlers
const char *CmdAddHandler(bloom_cell *bloom, const size_t hashes[])
{
    int i;
    for (i=0; i<k; i++)
        JumpBit(bloom, hashes[i]);
    return added_response;
}

const char *CmdCheckHandler(bloom_cell *bloom, const size_t hashes[])
{
    int i;
    for (i=0; i<k; i++)
        if (!GetBit(bloom, hashes[i]))
            return miss_response;
    return hit_response;
}

const char *CmdCheckThenAddHandler(bloom_cell *bloom, const size_t hashes[])
{
    bool present = true;
    int i;
    for (i=0; i<k; i++)
        if (!GetBit(bloom, hashes[i])) {
            present = false;
            break;
        }
    if (!present)
        for (i=0; i<k; i++)
            JumpBit(bloom, hashes[i]);
    return present ? hit_response : miss_response;
}

//Request routing
void* HandlerTable[][2] = {
{"/add",            CmdAddHandler           },
{"/check",          CmdCheckHandler         },
{"/checkthenadd",   CmdCheckThenAddHandler  },
};

//Storage operations
int SaveSnap(const bloom_cell *bloom, const char *fname)
{
    fputs("Saving snapshot...\n", stderr);
    size_t shouldwrite = (m + (CHAR_BIT - 1)) / CHAR_BIT;
    FILE *f = fopen(fname, "wb");
    if (f) {
        size_t bwritten = fwrite(bloom, 1, shouldwrite, f);
        if (bwritten != shouldwrite) {
            fprintf(stderr, "Should write: %ld bytes. Written %ld bytes.\n", shouldwrite, bwritten);
            return -1;
        }
        fclose(f);
        return 0;
    } else {
        fputs("Error opening file for writting.\n", stderr);
        return -1;
    }
}

int LoadSnap(bloom_cell *bloom, const char *fname)
{
    FILE *f = fopen(fname, "rb");
    if (f) {
        size_t shouldread = (m + (CHAR_BIT - 1)) / CHAR_BIT;
        size_t bread = fread(bloom, 1, shouldread, f);
        if (bread != shouldread) {
            fprintf(stderr, "Should read: %ld bytes. Read %ld bytes.\n", shouldread, bread);
            return -1;
        }
        fclose(f);
        return 0;
    } else {
        fputs("Error opening file for reading.\n", stderr);
        return -1;
    }
}

//Server globals
struct event_base *base = NULL;
struct evhttp *http = NULL;
struct evhttp_bound_socket *handle = NULL;
bloom_cell *Bloom = NULL;
pid_t dumper;
bool dumper_active = false;
char *snap_path = NULL;

//Signal handlers
void term_handler(int signo)
{
    event_base_loopexit(base, NULL);
}

void dump_handler(int signo)
{
    if (dumper_active)
        return;
    dumper_active = true;
    pid_t pid = fork();
    if (pid == 0) {
        if (snap_path && Bloom) {
            SaveSnap(Bloom, snap_path);
            exit(0);
        }
        else {
            fputs("Global pointers not set!\n", stderr);
            exit(11);
        }
    } else {
        if (pid == -1) {
            fputs("Unable to fork!\n", stderr);
            dumper_active = false;
        } else
        {
            dumper = pid;
        }
    }
}

void child_collector(int signo)
{
    int status;
    pid_t pid = waitpid(dumper, &status, WNOHANG);
    if (pid == dumper) {
        dumper_active = false;
        fprintf(stderr, "Dumper process %d exited with code %d. Collected him.\n", pid, WEXITSTATUS(status));
    }
}

//Main
int main(int argc, char *argv[])
{
    //Get args
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <snapshot_file>\n", argv[0]);
        return 1;
    }
    snap_path = argv[1];

    //Allocate memory
    fprintf(stderr, "Allocating arena with size %.2f MBytes ...\n", (float)m / CHAR_BIT / MEGA);
    Bloom = malloc( (m + ( CHAR_BIT - 1)) / CHAR_BIT ); // Ceil byte length: bytes = bits + 7 / 8


    //Load or create snapshot file
    if (!access(snap_path, F_OK)) {
        fputs("Loading snapshot...\n", stderr);
        if (LoadSnap(Bloom, snap_path)) {
            fputs("Unable to load snapshot!\n", stderr);
            return -1;
        }
        fputs("Snapshot loaded.\n", stderr);
    } else {
        fputs("Initializing new file storage...\n", stderr);
        size_t shouldwrite = (m + (CHAR_BIT - 1)) / CHAR_BIT;
        memset(Bloom, 0, shouldwrite); 

        if (SaveSnap(Bloom, snap_path)) {
            fputs("Unable to save initial snapshot!\n", stderr);
            return -1;
        }
        fputs("Initial snapshot written.\n", stderr);
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
        const char *path =  evhttp_uri_get_path(HTTPURI);
        if (!path) {
            evhttp_send_reply(req, HTTP_BADREQUEST, "Bad Request", OutBuf);
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

        int i;
        const char* (*Operation)(bloom_cell *, size_t []) = NULL;
        for (i=0; i< sizeof HandlerTable/ sizeof HandlerTable[0] ; i++)
            if (strncmp(HandlerTable[i][0], path, STR_MAX) == 0) {
                Operation = HandlerTable[i][1];
                break;
            }
        if (!Operation) {
            evhttp_clear_headers(&params);
            evhttp_send_reply(req, HTTP_NOTFOUND, "Not Found", OutBuf);
            return;
        }

        const char *response = Operation(Bloom, Hashes(element));

        evhttp_add_header(Headers, MIME_TYPE);
        evbuffer_add_printf(OutBuf, response);
        evhttp_send_reply(req, HTTP_OK, "OK", OutBuf);
        evhttp_clear_headers(&params);
    };

    base = event_base_new();
    if (!base) { 
        fputs("Couldn't create an event_base: exiting\n", stderr);
        return -1;
    }

    http = evhttp_new(base);
    if (!http) { 
        fputs("couldn't create evhttp. Exiting.\n", stderr);
        return -1;
    }
    evhttp_set_gencb(http, OnReq, NULL);

    handle = evhttp_bind_socket_with_handle(http, BIND_ADDRESS, BIND_PORT);
    if (!handle)
    { 
        fputs("couldn't bind to port 8888. Exiting.\n", stderr);
        return -1;
    }

    if (signal(SIGINT, term_handler) == SIG_ERR) {
        fputs("Unable to set SIGINT handler!", stderr);
        return -1;
    }
    if (signal(SIGTERM, term_handler) == SIG_ERR) {
        fputs("Unable to set SIGTERM handler!", stderr);
        return -1;
    }
    if (signal(SIGCHLD, child_collector) == SIG_ERR) {
        fputs("Unable to set SIGCHLD handler!", stderr);
        return -1;
    }
    if (signal(SIGUSR1, dump_handler) == SIG_ERR) {
        fputs("Unable to set SIGUSR1 handler!", stderr);
        return -1;
    }

    if (event_base_dispatch(base) == -1) {
        fputs("Failed to run message loop.\n", stderr);
        return -1;
    }

    fputs("Exiting...\n", stderr);
    SaveSnap(Bloom, snap_path);
    evhttp_del_accept_socket(http, handle);
    evhttp_free(http);
    event_base_free(base);
    free(Bloom);
    return 0;
}
