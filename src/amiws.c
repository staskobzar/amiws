/**
 * amiws -- Library with functions for read/create AMI packets
 * Copyright (C) 2017, Stas Kobzar <staskobzar@modulis.ca>
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
#include <signal.h>
#include "amiws.h"

/*! Default PID file when daemonizing */
#define DEFAULT_PID_FILE    "/tmp/amiws.pid"
/*! Default working directory when daemonizing */
#define DEFAULT_WORK_DIR    "/tmp"

static const struct option options[] = {
  "help",     0,  NULL, 'h',
  "file",     1,  NULL, 'f',
  "daemon",   0,  NULL, 'd',
  "pidfile",  1,  NULL, 'p',
  "wdir",     1,  NULL, 'D',
  /* sentinel */
  NULL,       0,  NULL, 0
};

static int exiting = 0;
static int pidfd = -1;
static char *conf_file = NULL;
static char *pidfile   = DEFAULT_PID_FILE;
static struct amiws_config *conf = NULL;

static void usage();
static void daemonize(const char *pidfile, const char *wdir);
static void handle_signal(int sig);

/**
 * Main amiws enter point.
 */
int main(int argc, const char *argv[])
{
  int c,
      option_index = 0,
      daemon       = 0;
  char *wdir      = DEFAULT_WORK_DIR;

  // parsing argument parameters
  while(1) {
    option_index = 0;
    c = getopt_long (argc, (char**)argv, "hdf:p:D:", options, &option_index);
    if(c == -1) break; // end of options list
    switch(c) {
      case 'h':
        usage();
        exit(EXIT_SUCCESS);
        break;

      case 'f': conf_file = optarg; break;

      case 'd': daemon = 1; break;

      case 'p': pidfile = optarg; break;

      case 'D': wdir = optarg; break;

      default:
        fprintf (stderr, "ERROR: Invalid option -%c.\n", c);
        exit(EXIT_FAILURE);
        break;
    }
  }

  if (daemon == 1) daemonize(pidfile, wdir);

  if ((conf = read_conf(conf_file == NULL ? DEFAULT_CONF_FILE : conf_file)) == NULL) {
    fprintf (stderr, "ERROR: Failed to read config.\n");
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, handle_signal);
  signal(SIGHUP, handle_signal);

  amiws_init(conf);

  for(;;) amiws_loop();

  return 0;
}


static void daemonize(const char *pidfile,
                      const char *wdir)
{
  pid_t pid, sid;
  char pidstr[12];

  syslog (LOG_INFO, "Daemonizing amiws process.");
  syslog (LOG_DEBUG, "Working directory: '%s'.", wdir);
  syslog (LOG_DEBUG, "PID file: '%s'.", pidfile);

  pid = fork();
  if (pid < 0) {
    syslog(LOG_ERR, "Failed to fork process.");
    exit(EXIT_FAILURE);
  }

  if (pid > 0) { // parent process
    syslog(LOG_DEBUG, "Daemon is created.");
    exit(EXIT_SUCCESS);
  }

  /* child proc */
  umask(0);

  sid = setsid();
  if (sid < 0) {
    syslog(LOG_DEBUG, "Daemon child failed to create SID.");
    exit(EXIT_FAILURE);
  }
  syslog(LOG_DEBUG, "Process session ID is %d", sid);

  /* Ignore signal sent from child to parent process */
  signal(SIGCHLD, SIG_IGN);

  if (chdir(wdir) < 0) {
    syslog(LOG_DEBUG, "Can not change to the working directory '%s'.", wdir);
    exit(EXIT_FAILURE);
  }
  syslog(LOG_DEBUG, "Working directory changed to %s", wdir);

  /* write PID file */
  pidfd = open(pidfile, O_RDWR|O_CREAT, 0600);
  if (pidfd < 0) {
    syslog(LOG_DEBUG, "Failed to open/create PID file '%s'", pidfile);
    exit(EXIT_FAILURE);
  }
  syslog(LOG_DEBUG, "Openned file %s to write PID", pidfile);

  if (lockf(pidfd, F_TLOCK, 0) < 0) {
    syslog(LOG_DEBUG, "Failed to lock PID file '%s'", pidfile);
    exit(EXIT_FAILURE);
  }

  sprintf(pidstr, "%d\n", getpid());
  if(write(pidfd, pidstr, strlen(pidstr)) < 0){
    syslog(LOG_ERR, "Failed to write to PID file.");
  }
  close(pidfd);

  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
}

int isExiting() {
    return exiting;
}

void setExiting() {
    exiting = 1;
}

static void handle_signal(int sig)
{
  switch(sig)
  {
    case SIGHUP:
      syslog(LOG_WARNING, "Received SIGHUP signal.");
      break;
    case SIGINT:
    case SIGTERM:
      // Set EXITING flag
      setExiting();

      syslog(LOG_INFO, "Exiting application.");
        if (pidfd != -1) {
        lockf(pidfd, F_ULOCK, 0);
        close(pidfd);
      }
      /* Try to delete pidfile */
      if (pidfile != NULL) {
        unlink(pidfile);
      }
      amiws_destroy();
      if (conf) free_conf(conf);
      /* Reset signal handling to default behavior */
      signal(SIGINT, SIG_DFL);
      exit(EXIT_SUCCESS);
      break;
    default:
      syslog(LOG_WARNING, "Unhandled signal %s", strsignal(sig));
      break;
  }
}


static void usage()
{
  printf( "Usage: " PACKAGE " [OPTIONS]\n");
  printf("Asterisk Management Interface (AMI) to WebSocket converter.\n\n");

  printf("Options:\n");
  printf("-h, --help              Printf this help message.\n");
  printf("-f, --file FILENAME     Configuration file. By default '/etc/amiws.yaml'\n");
  printf("-d, --daemon            Daemonize process.\n");
  printf("-p, --pidfile FILENAME  PID file when run as daemon. By default '/tmp/amiws.pid'\n");
  printf("-D, --wdir PATH         Working directory when run as daemon. By default '/tmp'\n");

  printf("\n");
  printf( PACKAGE_STRING " Copyright (C) 2017  " PACKAGE_BUGREPORT "\n"
          "This program comes with ABSOLUTELY NO WARRANTY.\n"
          "This is free software, and you are welcome to redistribute it\n"
          "under conditions describer in COPYRIGHT file.\n");
}
