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
#include <yaml.h>

#include "amipack.h"
#include "config.h"
#include "frozen.h"
#include "mongoose.h"

#define DEFAULT_CONF_FILE     "/etc/amiws.yaml"
#define DEFAULT_WEB_ROOT      "./web_root"
#define DEFAULT_LOG_LEVEL     LOG_INFO
#define DEFAULT_LOG_FACILITY  LOG_SYSLOG
#define DEFAULT_WEBSOCK_PORT  8000

#define POLL_SLEEP 10000
#define BUFSIZE 1024 * 8

#define intval(val) str2int(val, strlen(val))

#define macro_init_conf(conf) conf = (struct amiws_config *) calloc(1, sizeof(struct amiws_config));\
  conf->log_level     = DEFAULT_LOG_LEVEL;    \
  conf->log_facility  = DEFAULT_LOG_FACILITY; \
  conf->ws_port       = DEFAULT_WEBSOCK_PORT; \
  conf->size          = 0;                    \
  conf->head          = NULL;                 \
  conf->tail          = NULL;                 \
  conf->web_root      = DEFAULT_WEB_ROOT;     \
  conf->auth_domain   = NULL;                 \
  conf->auth_file     = NULL;


#define macro_init_conn(conn) conn = (struct amiws_conn *) calloc(1, sizeof(struct amiws_conn)); \
  conn->port = 5038;      \
  conn->address = NULL;   \
  conn->name = NULL;      \
  conn->host = NULL;      \
  conn->username = NULL;  \
  conn->secret = NULL;    \


struct amiws_config {
  struct amiws_conn *head;
  struct amiws_conn *tail;
  int size;
  int log_facility;
  int log_level;
  int ws_port;
  char *web_root;
  char *auth_domain;
  char *auth_file;
#if MG_ENABLE_SSL
  char *ssl_cert;
  char *ssl_key;
#endif
};

struct amiws_conn {
  int id;
  char *name;
  char *address;
  unsigned int port;
  char *host;
  char *username;
  char *secret;
  AMIVer ami_ver;
  struct amiws_conn *next;
};

enum token_context {
  CXT_TKN_KEY,
  CXT_TKN_VALUE,
  CXT_BLOCK_START,
  CXT_BLOCK_END,
};

void amiws_init(struct amiws_config *conf);

void amiws_connect_ami_server(struct amiws_conn *conn);

void amiws_destroy();

void amiws_loop();

void ami_ev_handler(struct mg_connection *nc, int ev, void *ev_data);

void websock_ev_handler (struct mg_connection *nc, int ev, void *ev_data);

void websock_send (struct mg_connection *nc, const char *json);

void ami_login(struct mg_connection *nc, struct amiws_conn *conn);

char *amipack_to_json(const char *buf, struct amiws_conn *conn);

struct amiws_config *read_conf(const char *filename);

int scan_amipack(const char *pack, size_t len);

/* private methods */

static void read_buffer(struct mbuf *io, struct mg_connection *nc);

static void set_conf_param(struct amiws_config *conf, char *key, char *value);

static void set_conn_param(struct amiws_conn *conn, char *key, char *value);

static int str2int(const char *val, int len);

static char* int2str(const char *val, int len);

static int auth_fail(AMIPacket *amipack);

static void send_ami_action(struct websocket_message *wm, struct mg_connection *nc);

static void json_scan_cb(void *callback_data,
                          const char *name, size_t name_len,
                          const char *path,
                          const struct json_token *token);

static struct amiws_config *valid_conf(struct amiws_config *conf);

static void free_conf(struct amiws_config *conf);

static int is_valid_auth_settings(struct amiws_config *conf);

static int is_valid_ssl_settings(struct amiws_config *conf);

static void usage();

#endif
