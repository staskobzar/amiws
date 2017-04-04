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
 * @file amiws.h
 * @brief AMI (Asterisk Management Interface) connector and websocket
 * remote interface functions.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#ifndef __AMIWS_H
#define __AMIWS_H

#include <stdio.h>
#include <syslog.h>
#include <amip.h>

#include "config.h"
#include "frozen.h"
#include "mongoose.h"

#define POLL_SLEEP 10000
#define BUFSIZE 1024 * 8

struct amiws_config {
  struct amiws_conn *head;
  struct amiws_conn *tail;
  int size;
  int log_facility;
  int log_level;
  char *ws_port;
};

struct amiws_conn {
  char *name;
  char *address;
  unsigned int port;
  char *host;
  char *username;
  char *secret;
  AMIVer ami_ver;
  struct amiws_conn *next;
};

void amiws_init(struct amiws_config *conf);

void amiws_connect_ami_server(struct amiws_conn *conn);

void amiws_destroy();

void amiws_loop();

void ami_ev_handler(struct mg_connection *nc, int ev, void *ev_data);

void websock_ev_handler (struct mg_connection *nc, int ev, void *ev_data);

void websock_send (struct mg_connection *nc, const char *json);

void ami_login(struct mg_connection *nc, struct amiws_conn *conn);

char *amipack_to_json(const char *buf);

#endif
