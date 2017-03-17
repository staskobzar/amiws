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

  for(;;) amiws_loop();
  //amiws_destroy();


  return 0;
}
