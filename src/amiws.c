/**
 * amiws -- Library with functions for read/create AMI packets
 * Copyright (C) 2016, Stas Kobzar <staskobzar@modulis.ca>
 *
 * This file is part of amiws.
 *
 * amiws is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * amiws is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with amiws.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file amiws.c
 * @brief AMI/web-socket main file.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */
#include <getopt.h>
#include "amiws.h"

static const struct option options[] = {
  "help", 0,  NULL, 'h',
  "file", 1,  NULL, 'f',
  /* sentinel */
  NULL,   0,  NULL, 0
};

int main(int argc, const char *argv[])
{
  int c,
      option_index = 0;
  char *conf_file = NULL;
  struct amiws_config *conf = NULL;
free(conf_file);
  // parsing argument parameters
  while(1) {
    option_index = 0;
    c = getopt_long (argc, (char**)argv, "hf:", options, &option_index);
    if(c == -1) break; // end of options list
    switch(c) {
      case 'h':
        usage();
        exit(EXIT_SUCCESS);
        break;

      case 'f':
        conf_file = optarg;
        break;

      default:
        fprintf (stderr, "ERROR: Invalid option -%c.\n", c);
        exit(EXIT_FAILURE);
        break;
    }
  }

  if ((conf = read_conf(conf_file == NULL ? DEFAULT_CONF_FILE : conf_file)) == NULL) {
    fprintf (stderr, "ERROR: Failed to read config.\n");
    exit(EXIT_FAILURE);
  }

  atexit (amiws_destroy);
  amiws_init(conf);

  for(;;) amiws_loop();

  return 0;
}

static void usage()
{
  printf( "Usage: " PACKAGE " [OPTIONS]\n");
  printf("Asterisk Management Interface (AMI) to WebSocket converter.\n\n");

  printf("Options:\n");
  printf("-h, --help            Printf this help message.\n");
  printf("-f, --file FILENAME   Configuration file. By default '/etc/amiws.yaml'\n");

  printf("\n");
  printf( PACKAGE_STRING " Copyright (C) 2017  " PACKAGE_BUGREPORT "\n"
          "This program comes with ABSOLUTELY NO WARRANTY.\n"
          "This is free software, and you are welcome to redistribute it\n"
          "under conditions describer in COPYRIGHT file.\n");
}
