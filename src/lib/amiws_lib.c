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
 * @file amiws_lib.c
 * @brief AMI/web-socket functions.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "amiws.h"

static struct mg_mgr mgr;
static struct mg_serve_http_opts s_http_server_opts;
struct mg_connection *nc_ws;

void amiws_init(struct amiws_config *conf)
{
  char port_str[5];
  struct mg_bind_opts bind_opts;

  mg_mgr_init(&mgr, NULL);

  setlogmask (LOG_UPTO (conf->log_level));
  openlog(PACKAGE, LOG_PERROR | LOG_CONS | LOG_PID | LOG_NDELAY, conf->log_facility);

  syslog (LOG_INFO, "AMI hosts connecting.");
  for (struct amiws_conn* conn = conf->head; conn; conn = conn->next) {
    amiws_connect_ami_server(conn);
  }

  sprintf(port_str, "%d", conf->ws_port);
  syslog (LOG_INFO, "Listening Web Socket port %s", port_str);

  memset(&bind_opts, 0, sizeof(bind_opts));
#if MG_ENABLE_SSL
  if(conf->ssl_cert){
    syslog (LOG_INFO, "Enabling SSL on HTTP/Web-sock connection.");
    bind_opts.ssl_cert = conf->ssl_cert;
    bind_opts.ssl_key  = conf->ssl_key;
    syslog (LOG_DEBUG, "SSL cert file '%s', key file '%s'", bind_opts.ssl_cert, bind_opts.ssl_key);
  }
#endif
  nc_ws = mg_bind_opt(&mgr, port_str, websock_ev_handler, bind_opts);

  if(nc_ws == NULL) {
    syslog (LOG_ERR, "Failed to bind port %s...", port_str);
    exit(EXIT_FAILURE);
  }
  mg_set_protocol_http_websocket(nc_ws);
  s_http_server_opts.document_root = conf->web_root;
  if(conf->auth_file){
    syslog (LOG_INFO, "Enabling HTTP Basic authentication.");
    s_http_server_opts.global_auth_file = conf->auth_file;
    s_http_server_opts.auth_domain = conf->auth_domain;
  }
}


void amiws_connect_ami_server(struct amiws_conn *conn)
{
  struct mg_connection *mgcon;
  struct mg_connect_opts opts;
  syslog (LOG_DEBUG, "Connecting to %s -> %s", conn->name, conn->address);

  memset(&opts, 0, sizeof(opts));
#if MG_ENABLE_SSL
  if(conn->ssl_key && conn->ssl_cert) {
    opts.ssl_key = conn->ssl_key;
    opts.ssl_cert = conn->ssl_cert;
  syslog (LOG_DEBUG, "SSL is enabled for '%s'. [key: %s, cert: %s]",
      conn->name, conn->ssl_key, conn->ssl_cert);
  }
#endif
  mgcon = mg_connect_opt(&mgr, conn->address, ami_ev_handler, opts);
  if(mgcon == NULL){
    syslog (LOG_ERR, "Failed to connect server %s", conn->address);
  } else {
    mgcon->user_data = (void*) conn;
  }
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
  struct amiws_conn *conn = (struct amiws_conn *) nc->user_data;

  switch(ev) {
    case MG_EV_POLL:
      //printf("MG_EV_POLL\n");
      break;

    case MG_EV_CONNECT:
      syslog (LOG_DEBUG, "Event: MG_EV_CONNECT");
      break;

    case MG_EV_RECV:

      if ( amipack_parser_detect(io->buf) == AMI_PROMPT &&
           amipack_parser_prompt(io->buf, &conn->ami_ver) == 1 ) {

        syslog (LOG_INFO, "AMI version: %d.%d.%d", conn->ami_ver.major,
            conn->ami_ver.minor, conn->ami_ver.patch);
        ami_login(nc, conn);
        mbuf_remove(io, io->len);
        break;

      }

      read_buffer(io, nc);

      if (io->len > 0) {
        syslog (LOG_DEBUG, "Stream tail: %.*s", (int)io->len, io->buf);
      }

      break;

    case MG_EV_SEND:
      // sent data
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


char *amipack_to_json(const char *ami_pack_str, struct amiws_conn *conn)
{
  char    *buf          = (char *) malloc(BUFSIZE);
  struct  json_out out  = JSON_OUT_BUF(buf, BUFSIZE);

  enum pack_type type = amipack_parser_detect(ami_pack_str);

  json_printf(&out,
      "{ type: %d, server_id: %d, server_name: %Q, ssl: %B, data: {",
      type, conn->id, conn->name, conn->is_ssl);

  switch(type)
  {
    case AMI_ACTION:
    case AMI_EVENT:
    case AMI_RESPONSE:
    case AMI_RESPCMD: {
      AMIPacket *ami_pack;
      if ((ami_pack = amipack_parser_message(ami_pack_str)) == NULL) {
        syslog (LOG_ERR, "Failed to parse pack: %s", ami_pack_str);
        return NULL;
      }
      if (auth_fail(ami_pack)) {
        syslog (LOG_ERR, "Authentication failed.");
      }
      for (AMIHeader *hdr = ami_pack->head; hdr; hdr = hdr->next) {
        json_printf(&out, "%Q: %Q", hdr->name, hdr->value);
        if (hdr != ami_pack->tail) {
          json_printf(&out, ",");
        }
      }
      json_printf(&out, "}}", 1);
      amipack_destroy(ami_pack);
      break;
    }

    case AMI_QUEUE: {
      AMIQueue *queue;
      if ((queue = amipack_parser_queue(ami_pack_str)) == NULL) {
        syslog (LOG_ERR, "Failed to parse queue: %s", ami_pack_str);
        return NULL;
      }
      json_printf(&out, "QueueName: %Q,", queue->name);
      json_printf(&out, "Calls: %d,", queue->calls);
      json_printf(&out, "Maxlen: %d,", queue->maxlen);
      json_printf(&out, "Strategy: %Q,", queue->strategy);
      json_printf(&out, "Holdtime: %d,", queue->holdtime);
      json_printf(&out, "Talktime: %d,", queue->talktime);
      json_printf(&out, "Weight: %d,", queue->weight);
      json_printf(&out, "CallsCompleted: %d,", queue->callscompleted);
      json_printf(&out, "CallsAbandoned: %d,", queue->callsabandoned);
      json_printf(&out, "ServiceLevel: %Q,", queue->sl);
      json_printf(&out, "ServiceLevelPeriod: %d,", queue->sl_sec);
      json_printf(&out, "MembersCount: %d,", queue->members_size);
      json_printf(&out, "CallersCount: %d", queue->callers_size);
      json_printf(&out, "}}", 1);
      amipack_queue_destroy(queue);
      break;
    }
    default:
      syslog (LOG_ERR, "Unknown AMI packet:\n%s", ami_pack_str);
      if (buf) free(buf);
      return NULL;
      break;
  }

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
  struct http_message *hm = (struct http_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST: {
      syslog (LOG_INFO, "New HTTP connection from %s:%d",
             inet_ntoa(nc->sa.sin.sin_addr), ntohs(nc->sa.sin.sin_port));
      mg_serve_http(nc, hm, s_http_server_opts); /* Serve static content */
      break;
    }
    case MG_EV_WEBSOCKET_HANDSHAKE_DONE: {
      syslog (LOG_INFO, "New Web Socket connection from %s:%d",
             inet_ntoa(nc->sa.sin.sin_addr), ntohs(nc->sa.sin.sin_port));
      // send state request to all
      break;
    }
    case MG_EV_WEBSOCKET_FRAME: {
      struct websocket_message *wm = (struct websocket_message *) ev_data;
      /* New websocket message. Tell everybody. */
      struct mg_str d = {(char *) wm->data, wm->size};
      syslog (LOG_DEBUG, "WebSock Msg received: %.*s", (int)d.len, d.p);
      send_ami_action(wm, nc);
      break;
    }
    case MG_EV_CLOSE: {
      syslog (LOG_INFO, "Closed New Web Socket connection from %s:%d",
             inet_ntoa(nc->sa.sin.sin_addr), ntohs(nc->sa.sin.sin_port));
      break;
    }
  }
}


void ami_login(struct mg_connection *nc, struct amiws_conn *conn)
{
  char *pack_str;
  size_t len;
  AMIPacket *pack = (AMIPacket *) amipack_init ();
  amipack_type(pack, AMI_ACTION);
  amipack_append_action (pack, strdup("Login"), 5);
  amipack_append (pack, strdup("Username"), 8, strdup(conn->username), strlen(conn->username));
  amipack_append (pack, strdup("Secret"), 6, strdup(conn->secret), strlen(conn->secret));

  len = amipack_to_str(pack, &pack_str);

  syslog (LOG_INFO, "AMI login with user: %s to server: %s", conn->username, conn->name);
  mg_send(nc, pack_str, len);

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
#if MG_ENABLE_SSL
  conf->ssl_cert      = NULL;
  conf->ssl_key       = NULL;
#endif

  while(1) {
    if (!yaml_parser_scan(&parser, &token)) {
      fprintf(stderr, "ERROR: Failed to parse token.\n");
      break;
    }

    if (token.type == YAML_KEY_TOKEN) context = CXT_TKN_KEY;
    if (token.type == YAML_VALUE_TOKEN) context = CXT_TKN_VALUE;
    if (token.type == YAML_BLOCK_ENTRY_TOKEN) {
      macro_init_conn(conn);
#if MG_ENABLE_SSL
      conn->ssl_cert      = NULL;
      conn->ssl_key       = NULL;
#endif
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
      conn->id = conf->size;
      block = CXT_BLOCK_END;
    }
    if (token.type == YAML_SCALAR_TOKEN) {
      if (context == CXT_TKN_KEY) {
        key = strdup((const char*)token.data.scalar.value);
      } else {
        val = strdup((const char*)token.data.scalar.value);
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


static void read_buffer(struct mbuf *io, struct mg_connection *nc)
{
  char *json = NULL, *buf = NULL;
  int len;


  struct amiws_conn *conn = (struct amiws_conn*) nc->user_data;
  syslog (LOG_DEBUG, "Reading buffer from server: %s", conn->name);

  while((len = scan_amipack(io->buf, io->len)) > 0) {

    if (io->len < len) {
      syslog (LOG_DEBUG, "io->len(%d) < len(%d). Skip to align.", (int)io->len, len);
      break;
    }

    buf = strndup(io->buf, len);
    json = amipack_to_json(buf, conn);
    if (json != NULL) {
      syslog (LOG_DEBUG, "JSON STRING: %s", json);
      websock_send (nc, json);
    }

    if(json) free(json);
    if(buf)  free(buf);
    mbuf_remove(io, len);
  }
}


int scan_amipack( const char *p,
                  size_t len)
{
  int i = 0, found = 0;
  while(i < len && !found){
    found = (p[i] == '\r' && p[i+1] == '\n' && p[i+2] =='\r' && p[i+3] == '\n');
    i++;
  }
  return found ? i + 3 : 0;
}


static void send_ami_action(struct websocket_message *wm,
                            struct mg_connection *nc)
{
  char *p_str = NULL;
  size_t len;
  struct mg_connection *c;
  char buf[BUFSIZE] = "";
  AMIPacket *pack = (AMIPacket *) amipack_init ();

  if(json_walk((const char*)wm->data, wm->size, json_scan_cb, (void*)pack) < (int)wm->size ){
    syslog (LOG_ERR, "Invalid JSON string: %.*s", (int)wm->size, wm->data);
    amipack_destroy(pack);
    return;
  }

  len = amipack_to_str(pack, &p_str);
  syslog (LOG_DEBUG, "Converted to AMI packet from JSON:\n %.*s", (int)len, p_str);

  // send to all AMI connection
  for (c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c)) {
    if(c->flags & MG_F_IS_WEBSOCKET) continue;
    mg_send(c, p_str, len);
  }

  if(p_str) free(p_str);
  amipack_destroy(pack);
}


static void json_scan_cb( void *callback_data,
                          const char *name, size_t name_len,
                          const char *path,
                          const struct json_token *token)
{
  AMIPacket *pack = (AMIPacket*) callback_data;
  switch(token->type) {
    case JSON_TYPE_STRING:
    case JSON_TYPE_NUMBER:
    case JSON_TYPE_TRUE:
    case JSON_TYPE_FALSE:
    case JSON_TYPE_NULL:
      amipack_append(pack, strndup(name, name_len), name_len,
                           strndup(token->ptr, token->len), token->len);
      break;
    default: break;
  }
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

  } else if (strcmp(key, "web_root") == 0) {

    conf->web_root = val;

  } else if (strcmp(key, "auth_domain") == 0) {

    conf->auth_domain = val;

  } else if (strcmp(key, "auth_file") == 0) {

    conf->auth_file = val;

#if MG_ENABLE_SSL
  } else if (strcmp(key, "ssl_cert") == 0) {

    conf->ssl_cert = val;

  } else if (strcmp(key, "ssl_key") == 0) {

    conf->ssl_key = val;
#endif
  } else {
    fprintf(stderr, "WARNING: >> Unknown parameter '%s: %s'.\n", key, val);
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

#if MG_ENABLE_SSL
  } else if (strcmp(key, "ssl_key") == 0) {

    conn->ssl_key = val;

  } else if (strcmp(key, "ssl_cert") == 0) {

    conn->ssl_cert = val;
#endif

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


static int auth_fail(AMIPacket *pack)
{
  char *resp;
  size_t len;

  if(pack->type != AMI_RESPONSE)
    return 0;

  if((len = amiheader_find(pack, "Response", &resp)) == -1){
    return 0;
  }

  if(strncasecmp(resp, "Error", len) != 0) {
    return 0;
  }

  if((len = amiheader_find(pack, "Message", &resp)) == -1) {
    return 0;
  }

  if(strncasecmp(resp, "Authentication failed", len) == 0) {
    return 1;
  }
  return 0;
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

  // auth settings
  if ((conf->auth_domain != NULL || conf->auth_file != NULL) &&
      !is_valid_auth_settings(conf) ) err = 1;
  // SSL files
#if MG_ENABLE_SSL
  if ((conf->ssl_key != NULL || conf->ssl_cert != NULL) &&
      !is_valid_ssl_settings(conf) ) err = 1;
#endif

  for (struct amiws_conn *conn = conf->head; conn; conn = conn->next) {
    char address[256] = "";
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
#if MG_ENABLE_SSL
    if (!err && (conn->ssl_key != NULL || conn->ssl_cert != NULL)){
      conn->is_ssl = is_valid_ssl_conn_settings(conn);
      err = !is_valid_ssl_conn_settings(conn);
    }
#endif
    if(!err) {
      sprintf(address, "tcp://%s:%d", conn->host, conn->port);
      conn->address = strdup(address);
    }
  }

  // if error found - free conf struct and return NULL
  if(err) { free_conf(conf); }

  return err ? NULL : conf;
}


static int is_valid_auth_settings(struct amiws_config *conf)
{
  if(conf->auth_domain == NULL) {
    fprintf(stderr, "ERROR: Auth domain parameter requires auth file to be set too.\n");
    return 0;
  }
  if(conf->auth_file == NULL) {
    fprintf(stderr, "ERROR: Auth file parameter requires auth domain to be set too.\n");
    return 0;
  }
  if(access(conf->auth_file, F_OK|R_OK) == -1) {
    fprintf(stderr, "ERROR: Failed to read auth file '%s'.\n", conf->auth_file);
    return 0;
  }
  return 1;
}


#if MG_ENABLE_SSL
static int is_valid_ssl_settings(struct amiws_config *conf)
{
  if(conf->ssl_key == NULL){
    fprintf(stderr, "ERROR: When SSL certificate is given then SSL key (ssl_key) must be set too.\n");
    return 0;
  }
  if(conf->ssl_cert == NULL){
    fprintf(stderr, "ERROR: When SSL key is given then SSL certificate (ssl_cert) must be set too.\n");
    return 0;
  }
  if(access(conf->ssl_cert, F_OK|R_OK) == -1) {
    fprintf(stderr, "ERROR: Failed to read SSL certificate file '%s'.\n", conf->ssl_cert);
    return 0;
  }
  if(access(conf->ssl_key, F_OK|R_OK) == -1) {
    fprintf(stderr, "ERROR: Failed to read SSL key file '%s'.\n", conf->ssl_key);
    return 0;
  }
  return 1;
}


static int is_valid_ssl_conn_settings(struct amiws_conn *conn)
{
  if(conn->ssl_key == NULL){
    fprintf(stderr, "ERROR: When SSL certificate is given for connection '%s' then SSL key (ssl_key) must be set too.\n",
        conn->name);
    return 0;
  }
  if(conn->ssl_cert == NULL){
    fprintf(stderr, "ERROR: When SSL key is given for connection '%s' then SSL certificate (ssl_cert) must be set too.\n",
        conn->name);
    return 0;
  }
  if(access(conn->ssl_cert, F_OK|R_OK) == -1) {
    fprintf(stderr, "ERROR: Failed to read SSL certificate file '%s' for server '%s'.\n",
        conn->ssl_cert, conn->name);
    return 0;
  }
  if(access(conn->ssl_key, F_OK|R_OK) == -1) {
    fprintf(stderr, "ERROR: Failed to read SSL key file '%s' for server '%s'.\n",
        conn->ssl_key, conn->name);
    return 0;
  }
  return 1;
}
#endif // end of MG_ENABLE_SSL


void free_conf(struct amiws_config *conf)
{
  if(!conf) return;
  for (struct amiws_conn *conn = conf->head; conn; conn = conn->next) {
    if(conn->name) free(conn->name);
    if(conn->address) free(conn->address);
    if(conn->host) free(conn->host);
    if(conn->username) free(conn->username);
    if(conn->secret) free(conn->secret);
#if MG_ENABLE_SSL
    if(conn->ssl_cert) free(conn->ssl_cert);
    if(conn->ssl_key) free(conn->ssl_key);
#endif
  }

  if (conf){
    if(conf->web_root) free(conf->web_root);
    if(conf->auth_domain) free(conf->auth_domain);
    if(conf->auth_file) free(conf->auth_file);
#if MG_ENABLE_SSL
    if(conf->ssl_cert) free(conf->ssl_cert);
    if(conf->ssl_key) free(conf->ssl_key);
#endif
    free(conf);
  }
}

