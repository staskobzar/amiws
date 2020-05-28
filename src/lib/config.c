#include <stdio.h>
#include <yaml.h>

#include "config_parser.h"
#include "amiws.h"

void* ParseAlloc(void* (*allocProc)(size_t));
void* Parse(void*, int, const char*, struct amiws_config *);
void* ParseFree(void*, void(*freeProc)(void*));

static unsigned int yaml_tokenize(yaml_token_type_t token)
{
  switch(token) {
    case YAML_NO_TOKEN                   :  return 0;
    case YAML_STREAM_START_TOKEN         :  return STREAM_START;
    case YAML_STREAM_END_TOKEN           :  return STREAM_END;

    case YAML_BLOCK_MAPPING_START_TOKEN  :  return BLOCK_MAPPING_START;
    case YAML_BLOCK_END_TOKEN            :  return BLOCK_END;

    case YAML_SCALAR_TOKEN               :  return SCALAR;
    case YAML_KEY_TOKEN                  :  return KEY;
    case YAML_VALUE_TOKEN                :  return VALUE;

    case YAML_BLOCK_SEQUENCE_START_TOKEN :  return BLOCK_SEQUENCE_START;
    case YAML_BLOCK_ENTRY_TOKEN          :  return BLOCK_ENTRY;
    default: return 0;
  }
}


struct amiws_config *read_conf(const char *filename)
{
  FILE *fh;
  yaml_parser_t parser;
  yaml_token_t  token;
  struct amiws_config *conf;
  char *val;
  void *scanner = ParseAlloc (malloc);

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

  // scanning
  do {
    if (!yaml_parser_scan(&parser, &token)) {
      fprintf(stderr, "ERROR: Failed to parse token.\n");
      goto done;
    }
    if(token.type == YAML_NO_TOKEN) {
      // parsing fail - invalid config file
      goto done;
    }

    if ( token.type == YAML_SCALAR_TOKEN ) {
      val = (char*)strndup(token.data.scalar.value, token.data.scalar.length);
    } else { val = NULL; }

    Parse(scanner, yaml_tokenize(token.type), val, conf);

    if (conf->parse_fail) {
      fprintf(stderr, "ERROR: Failed to parse file.\n");
      goto done;
    }
    if (conf->syntax_error) {
      fprintf(stderr, "ERROR: Config file syntax error.\n");
      goto done;
    }

    if(token.type != YAML_STREAM_END_TOKEN)
      yaml_token_delete(&token);

  } while(token.type != YAML_STREAM_END_TOKEN);


  Parse (scanner, 0, 0, NULL);

done:
  ParseFree (scanner, free);
  yaml_token_delete(&token);
  yaml_parser_delete(&parser);
  fclose(fh);
  return valid_conf(conf);
}


void set_conf_param( struct amiws_config *conf,
                            char *key,
                            char *val)
{
    if (strcmp(key, "log_level") == 0) {

    conf->log_level = intval(val);
    if (conf->log_level == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);
    free(val);

  } else if (strcmp(key, "log_facility") == 0) {

    conf->log_facility = intval(val);
    if (conf->log_facility == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);
    free(val);

  } else if (strcmp(key, "ws_port") == 0) {

    conf->ws_port = intval(val);
    if (conf->ws_port == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);
    free(val);

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
    free(val);
  }
  free(key);
}


void set_conn_param( struct amiws_conn *conn,
                            char *key,
                            char *val)
{
  if (strcmp(key, "name") == 0) {

    conn->name = val;

  } else if (strcmp(key, "id") == 0) {

    conn->id = intval(val);

  } else if (strcmp(key, "port") == 0) {

    conn->port = intval(val);
    if (conn->port == -1)
      fprintf(stderr, "ERROR: Invalid %s: '%s'.\n", key, val);
    free(val);

  } else if (strcmp(key, "host") == 0) {

    conn->host = val;

  } else if (strcmp(key, "username") == 0) {

    conn->username = val;

  } else if (strcmp(key, "secret") == 0) {

    conn->secret = val;

  } else if (strcmp(key, "event_names") == 0) {

    conn->event_names = val;

#if MG_ENABLE_SSL
  } else if (strcmp(key, "ssl_key") == 0) {

    conn->ssl_key = val;

  } else if (strcmp(key, "ssl_cert") == 0) {

    conn->ssl_cert = val;
#endif

  } else {
    fprintf(stderr, "ERROR: Unknown parameter '%s: %s'.\n", key, val);
    free(val);
  }
  free(key);
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


void free_conf(struct amiws_config *conf)
{
  if(!conf) return;
  for (struct amiws_conn *conn = conf->head; conn; conn = conn->next) {
    if(conn->name)      free(conn->name);
    if(conn->address)   free(conn->address);
    if(conn->host)      free(conn->host);
    if(conn->username)  free(conn->username);
    if(conn->secret)    free(conn->secret);
#if MG_ENABLE_SSL
    if(conn->ssl_cert)  free(conn->ssl_cert);
    if(conn->ssl_key)   free(conn->ssl_key);
#endif
    free(conn);
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
  if (conf->web_root == NULL) conf->web_root = strdup(DEFAULT_WEB_ROOT);
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

