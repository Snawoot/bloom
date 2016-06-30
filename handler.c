#include <stdint.h>
#include <evhttp.h>
#include <string.h>
#include "bf_types.h"
#include "commands.h"
#include "globals.h"

#define MIME_TYPE "Content-Type", "text/html; charset=UTF-8"

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
    const char* (*Operation)(bloom_filter_t *, const char []) = NULL;
    for (i=0; i< sizeof HandlerTable/ sizeof HandlerTable[0] ; i++)
        if (strcmp(HandlerTable[i][0], path) == 0) {
            Operation = HandlerTable[i][1];
            break;
        }
    if (!Operation) {
        evhttp_clear_headers(&params);
        evhttp_send_reply(req, HTTP_NOTFOUND, "Not Found", OutBuf);
        return;
    }

    const char *response = Operation(Bloom, element);

    evhttp_add_header(Headers, MIME_TYPE);
    evbuffer_add(OutBuf, response, strlen(response));
    evhttp_send_reply(req, HTTP_OK, "OK", OutBuf);
    evhttp_clear_headers(&params);
};
