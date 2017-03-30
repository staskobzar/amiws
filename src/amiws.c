#include "amiws.h"

int main(int argc, const char *argv[])
{

  struct amiws_config *conf = (struct amiws_config *) calloc(1, sizeof(struct amiws_config));
  struct amiws_conn *conn1  = (struct amiws_conn *) calloc(1, sizeof(struct amiws_conn));
  //struct amiws_conn *conn2  = (struct amiws_conn *) calloc(1, sizeof(struct amiws_conn));

  conf->head = conn1;
  conf->tail = conn1;
  conf->size = 1;
  conn1->name = "gw05.mor";
  conn1->port = 5038;
  conn1->host = "192.168.1.105";
  conn1->address = "tcp://192.168.1.105:5038";
  conn1->username = "admin";
  conn1->secret = "ulaval";
  conn1->next = NULL;

  /*
  conf->head = conn1;
  conf->tail = conn2;
  conf->size = 2;

  conn1->name = "gw05.mor";
  conn1->port = 5038;
  conn1->host = "127.0.0.1";
  conn1->address = "tcp://127.0.0.1:5038";
  conn1->username = "gw05";
  conn1->secret = "ulaval";
  conn1->next = conn2;

  conn2->name = "gw07.mor";
  conn2->port = 5039;
  conn2->host = "127.0.0.1";
  conn2->address = "tcp://127.0.0.1:5039";
  conn2->username = "gw07";
  conn2->secret = "ulaval";
  conn2->next = NULL;
  */

  atexit (amiws_destroy);
  amiws_init(conf);

  /*
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
  */

  for(;;) amiws_loop();
  //amiws_destroy();

  //free(conn1);
  //free(conn2);
  //free(conf);
  return 0;
}
