#include "amiws.h"

int main(int argc, const char *argv[])
{
  struct amiws_config *conf = read_conf("/tmp/amiws.cfg");
  if (conf == NULL) {
    fprintf (stderr, "ERROR: Failed to read config.\n");
    return EXIT_FAILURE;
  }

  atexit (amiws_destroy);
  amiws_init(conf);

  for(;;) amiws_loop();

  return 0;
}
