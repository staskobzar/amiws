#include "amiws.h"

int main(int argc, const char *argv[])
{

  struct amiws_config *conf = (struct amiws_config *) calloc(1, sizeof(struct amiws_config));
  struct amiws_conn *conn1  = (struct amiws_conn *) calloc(1, sizeof(struct amiws_conn));
  //struct amiws_conn *conn2  = (struct amiws_conn *) calloc(1, sizeof(struct amiws_conn));

  conf->head      = conn1;
  conf->tail      = conn1;
  conf->size      = 1;
  conf->log_facility = LOG_SYSLOG;
  conf->log_level = LOG_DEBUG;
  conf->ws_port   = "8000";
  conn1->name     = "gw05.mor";
  conn1->port     = 5038;
  conn1->host     = "192.168.1.105";
  conn1->address  = "tcp://192.168.1.105:5038";
  conn1->username = "admin";
  conn1->secret   = "ulaval";
  conn1->next     = NULL;

  atexit (amiws_destroy);
  amiws_init(conf);

  for(;;) amiws_loop();

  return 0;
}
