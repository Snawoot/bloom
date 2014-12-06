#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/queue.h>
#include <evhttp.h>

char const SrvAddress[] = "0.0.0.0";
int SrvPort = 8888;

int main()
{
  if (!event_init())
  {
    fputs("Failed to init libevent.", stderr);
    return -1;
  }
  struct evhttp *Server = evhttp_start(SrvAddress, SrvPort);
  if (!Server)
  {
    fputs("Failed to init HTTP-server.", stderr);
    return -1;
  }

  void OnReq(struct evhttp_request *req, void *arg)
  {
    struct evbuffer *OutBuf = evhttp_request_get_output_buffer(req);
    if (!OutBuf)
      return;
    struct evkeyvalq *Headers = evhttp_request_get_output_headers(req);
    if (!Headers)
      return;

    evhttp_add_header(Headers, "Content-Type", "text/html; charset=UTF-8");
    evbuffer_add_printf(OutBuf, "<html><body><center><h1>Hello World!</h1></center></body></html>");
    evhttp_send_reply(req, HTTP_OK, "", OutBuf);
  };
  evhttp_set_gencb(Server, OnReq, 0);

  if (event_dispatch() == -1)
  {
    fputs("Failed to run message loop.", stderr);
    return -1;
  }

  evhttp_free(Server);
  return 0;
}
