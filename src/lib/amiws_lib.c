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
 * @file amiws_lib.c
 * @brief AMI/web-socket functions.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "amiws.h"

static struct mg_mgr mgr;
struct mg_connection *nc_ws;

void amiws_init(struct amiws_config *conf)
{
  char port_str[5];

  mg_mgr_init(&mgr, NULL);

  setlogmask (LOG_UPTO (conf->log_level));
  openlog(PACKAGE, LOG_PERROR | LOG_CONS | LOG_PID | LOG_NDELAY, conf->log_facility);

  syslog (LOG_INFO, "Initiate connection");
  for (struct amiws_conn* conn = conf->head; conn; conn = conn->next) {
    amiws_connect_ami_server(conn);
  }

  sprintf(port_str, "%d", conf->ws_port);
  syslog (LOG_INFO, "Listening Web Socket port %s", port_str);
  nc_ws = mg_bind(&mgr, port_str, websock_ev_handler);
  mg_set_protocol_http_websocket(nc_ws);
}

void amiws_connect_ami_server(struct amiws_conn *conn)
{
  struct mg_connection *mgcon;
  syslog (LOG_DEBUG, "Connecting to %s -> %s", conn->name, conn->address);
  mgcon = mg_connect(&mgr, conn->address, ami_ev_handler);
  mgcon->user_data = (void*) conn;
}

void amiws_destroy()
{
  syslog (LOG_INFO, "Stop %s", PACKAGE);
  mg_mgr_free(&mgr);
}

void amiws_loop()
{
  mg_mgr_poll(&mgr, POLL_SLEEP);
}

void ami_ev_handler(struct mg_connection *nc,
                    int ev,
                    void *ev_data)
{
  (void)ev_data;
  struct mbuf *io = &nc->recv_mbuf;
  char *json;
  struct amiws_conn *conn = (struct amiws_conn *) nc->user_data;

  switch(ev) {
    case MG_EV_POLL:
      //printf("MG_EV_POLL\n");
      break;
    case MG_EV_CONNECT:
      syslog (LOG_DEBUG, "MG_EV_CONNECT");
      break;
    case MG_EV_RECV:

      if ( amiparse_prompt(io->buf, &conn->ami_ver) == RV_SUCCESS ) {

        syslog (LOG_INFO, "AMI version: %d.%d.%d", conn->ami_ver.major,
            conn->ami_ver.minor, conn->ami_ver.patch);
        ami_login(nc, conn);
        mbuf_remove(io, io->len);

      } else if (amiparse_stanza(io->buf,io->len) == RV_SUCCESS) {

        json = amipack_to_json(io->buf);
        if (json != NULL) {
          syslog (LOG_DEBUG, "JSON STRING: %s", json);
          websock_send (nc, json);
        }
        mbuf_remove(io, io->len);

      } else {
        // else : if packet is not complete (no stanza CRLF CRLF)
        // then : break and wait till next part arrives
        syslog (LOG_DEBUG, "Incomplete AMI packet: %.*s", (int)io->len, io->buf);
      }

      break;
    case MG_EV_CLOSE:
      syslog (LOG_INFO, "MG_EV_CLOSE reconnect ... [%s] %s\n", conn->address, conn->name);
      amiws_connect_ami_server(conn);
      sleep(1);
      break;
    default:
      syslog (LOG_DEBUG, "Unhandled connection event: %d", ev);
      break;
  }
}

char *amipack_to_json(const char *ami_pack_str)
{
  AMIPacket *ami_pack;
  char    *buf          = (char *) malloc(BUFSIZE);
  struct  json_out out  = JSON_OUT_BUF(buf, BUFSIZE);
  int     len           = 0;

  if( (ami_pack = amiparse_pack(ami_pack_str)) == NULL ) {
    return NULL;
  }

  len += json_printf(&out, "{ type: %d, data: {", ami_pack->type);

  struct str *sp = amipack_to_str(ami_pack);

  for (AMIHeader *hdr = ami_pack->head; hdr; hdr = hdr->next) {
    len += json_printf(&out, "%Q: %Q", hdr->name->buf, hdr->value->buf);
    if (hdr != ami_pack->tail) {
      len += json_printf(&out, ",");
    }
  }

  len += json_printf(&out, "}}", 1);

  amipack_destroy(ami_pack);

  return buf;
}

void websock_send (struct mg_connection *nc, const char *json)
{
  struct mg_connection *c;
  int len = strlen(json);

  for (c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c)) {
    if (c == nc) continue; /* Don't send to the sender. */
    if (c->flags & MG_F_IS_WEBSOCKET) {
      mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, json, len);
    }
  }
}

void websock_ev_handler (struct mg_connection *nc, int ev, void *ev_data)
{
  (void)nc;
  switch (ev) {
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
      syslog (LOG_INFO, "New Web Socket connection from %s:%d",
             inet_ntoa(nc->sa.sin.sin_addr), ntohs(nc->sa.sin.sin_port));
      break;
    }
    case MG_EV_WEBSOCKET_FRAME: {
      struct websocket_message *wm = (struct websocket_message *) ev_data;
      /* New websocket message. Tell everybody. */
      struct mg_str d = {(char *) wm->data, wm->size};
      syslog (LOG_DEBUG, "WebSock Msg received: %.*s", (int)d.len, d.p);
      break;
    }
    case MG_EV_CLOSE: {
      syslog (LOG_INFO, "Closed New Web Socket connection from %s:%d",
             inet_ntoa(nc->sa.sin.sin_addr), ntohs(nc->sa.sin.sin_port));
      break;
    }
  }
}

// TODO: when login fails - stop connecting
void ami_login(struct mg_connection *nc, struct amiws_conn *conn)
{
  struct str *pack_str;
  AMIPacket *pack = (AMIPacket *) amipack_init ();
  amipack_type(pack, AMI_ACTION);
  amipack_append (pack, Action, "Login");
  amipack_append (pack, Username, conn->username);
  amipack_append (pack, Secret, conn->secret);

  pack_str = amipack_to_str (pack);

  syslog (LOG_INFO, "AMI login with user: %s to server: %s", conn->username, conn->name);
  mg_send(nc, pack_str->buf, pack_str->len);

  str_destroy (pack_str);
  amipack_destroy (pack);
}

struct amiws_config *read_conf(const char *filename)
{
  FILE *fh;
  yaml_parser_t parser;
  yaml_token_t  token;
  struct amiws_config *conf;
  struct amiws_conn *conn;
  char *key, *val;
  enum token_context context = CXT_TKN_KEY,
                     block;

  if (!yaml_parser_initialize (&parser)) {
    fprintf(stderr, "ERROR: Failed to initialize YAML parser.\n");
    return NULL;
  }

  if ( (fh = fopen( filename, "rb" )) == NULL ) {
    fprintf(stderr, "ERROR: file \"%s\" does not exists or can not be read.\n", filename);
    return NULL;
  }

  yaml_parser_set_input_file (&parser, fh);

  macro_init_conf(conf);

  while(1) {
    if (!yaml_parser_scan(&parser, &token)) {
      fprintf(stderr, "ERROR: Failed to parse token.\n");
      break;
    }

    if (token.type == YAML_KEY_TOKEN) context = CXT_TKN_KEY;
    if (token.type == YAML_VALUE_TOKEN) context = CXT_TKN_VALUE;
    if (token.type == YAML_BLOCK_ENTRY_TOKEN) {
      macro_init_conn(conn);
      block = CXT_BLOCK_START;
    }
    if (token.type == YAML_BLOCK_END_TOKEN && block == CXT_BLOCK_START) {
      if (conf->size == 0) {
        conf->head = conn;
        conf->tail = conn;
        conn->next = NULL;
      } else {
        conf->tail->next = conn;
        conf->tail = conn;
      }
      conf->size++;
      block = CXT_BLOCK_END;
    }
    if (token.type == YAML_SCALAR_TOKEN) {
      if (context == CXT_TKN_KEY) {
        key = strdup(token.data.scalar.value);
      } else {
        val = strdup(token.data.scalar.value);
        if (block == CXT_BLOCK_START) { set_conn_param(conn, key, val); }
        else { set_conf_param(conf, key, val); }
      }
    }
    if( token.type == YAML_STREAM_END_TOKEN ) break;
    yaml_token_delete(&token);
  }

  yaml_parser_delete(&parser);
  fclose(fh);
  return valid_conf(conf);
}

static void set_conf_param( struct amiws_config *conf,
                            char *key,
                            char *val)
{
  if (strcmp(key, "log_level") == 0) {

    conf->log_level = intval(val);
    if (conf->log_level == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);

  } else if (strcmp(key, "log_facility") == 0) {

    conf->log_facility = intval(val);
    if (conf->log_facility == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);

  } else if (strcmp(key, "ws_port") == 0) {

    conf->ws_port = intval(val);
    if (conf->ws_port == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);

  } else {
    fprintf(stderr, "Unknown parameter '%s: %s'.\n", key, val);
  }
}

static void set_conn_param( struct amiws_conn *conn,
                            char *key,
                            char *val)
{
  if (strcmp(key, "name") == 0) {

    conn->name = val;

  } else if (strcmp(key, "port") == 0) {

    conn->port = intval(val);
    if (conn->port == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);

  } else if (strcmp(key, "host") == 0) {

    conn->host = val;

  } else if (strcmp(key, "username") == 0) {

    conn->username = val;

  } else if (strcmp(key, "secret") == 0) {

    conn->secret = val;

  } else if (strcmp(key, "amihosts") == 0) {

    fprintf(stderr, "INFO: Parsing hosts block.\n");

  } else {
    fprintf(stderr, "ERROR: Unknown parameter '%s: %s'.\n", key, val);
  }
}

static int str2int( const char *val, int len)
{
  int res = 0, i = 0, iv = 0;

  if (!len) return -1;

  for (i = 0; i < len; i++) {

    iv = val[i] - '0';

    if (iv < 0 || iv > 10) return -1;

    res = (res * 10) + iv;

  }

  return res;
}

static struct amiws_config *valid_conf(struct amiws_config *conf)
{
  int err = 0;
  if (  conf->log_level    == -1 ||
        conf->log_facility == -1 ||
        conf->ws_port      == -1) {
    free_conf(conf);
    return NULL;
  }
  for (struct amiws_conn *conn = conf->head; conn; conn = conn->next) {
    if (conn->name == NULL) {
      fprintf(stderr, "ERROR: Missing 'name' parameter for host.\n");
      err = 1;
    }
    if (conn->port == -1) {
      err = 1;
    }
    if (conn->host == NULL) {
      fprintf(stderr, "ERROR: Missing 'host' parameter for %s.\n", conn->name);
      err = 1;
    }
    if (conn->username == NULL) {
      fprintf(stderr, "ERROR: Missing 'username' parameter for %s.\n", conn->name);
      err = 1;
    }
    if (conn->secret == NULL) {
      fprintf(stderr, "ERROR: Missing 'secret' parameter for host %s.\n", conn->name);
      err = 1;
    }
  }
  if(err) free_conf(conf);
  return err ? NULL : conf;
}

static void free_conf(struct amiws_config *conf)
{
  for (struct amiws_conn *conn = conf->head; conn; conn = conn->next) {
    free(conn);
  }
  if (conf)
    free(conf);
}
