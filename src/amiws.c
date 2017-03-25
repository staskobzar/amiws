#include "amiws.h"

int main(int argc, const char *argv[])
{

  struct amiws_params params;
  params.port = 5038;
  params.host = "127.0.0.1";
  params.address = "tcp://127.0.0.1:5038";

  atexit (amiws_destroy);
  amiws_init(&params);

  amiws_loop(); // connect
  amiws_loop(); // poll
  amiws_loop();
  amiws_loop();
  amiws_loop();
  amiws_loop();
  amiws_loop();
  amiws_loop();
  amiws_loop();
  amiws_loop();
  amiws_loop();
  amiws_loop();

  //for(;;) amiws_loop();
  //amiws_destroy();
  /*
  struct mbuf buf;
  mbuf_init(&buf, 0);
  printf("buf->len = %lu; buf->size = %lu\n", buf.len, buf.size);
  mbuf_append(&buf, "Hello ", 6);
  printf("buf->len = %lu; buf->size = %lu\n", buf.len, buf.size);
  mbuf_append(&buf, "World", 5);
  printf("buf->len = %lu; buf->size = %lu\n", buf.len, buf.size);
  mbuf_append(&buf, "\n", 1);
  printf("buf->len = %lu; buf->size = %lu\n", buf.len, buf.size);
  printf("buf = %.*s", buf.len, buf.buf);

  mbuf_free(&buf);

  struct mbuf *nbuf = (struct mbuf*) malloc(sizeof(struct mbuf));
  mbuf_init(nbuf,0);
  printf("nbuf->len = %lu; nbuf->size = %lu\n", nbuf->len, nbuf->size);
  mbuf_append(nbuf, "Good night, ", 12);
  printf("nbuf->len = %lu; nbuf->size = %lu\n", nbuf->len, nbuf->size);
  mbuf_append(nbuf, "Sweet Prince!\n", 14);
  printf("nbuf->len = %lu; nbuf->size = %lu\n", nbuf->len, nbuf->size);
  printf("nbuf = %.*s", nbuf->len, nbuf->buf);
  mbuf_free(nbuf);
  free(nbuf);
  */

  return 0;
}
