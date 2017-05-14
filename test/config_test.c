#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "amiws.h"

static void config_file_does_not_exists (void **state)
{
  (void) *state;
  assert_null(read_conf ("/the/file/that_does_not.exists"));
}

static void config_file_with_invalid_int_value (void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/invalid_int_value.yml");
  assert_null(conf);
}

static void config_file_with_single_host(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/single_ami_host.yml");
  struct amiws_conn *conn = conf->head;
  assert_int_equal(conf->log_facility, LOG_LOCAL4);
  assert_int_equal(conf->log_level,    LOG_DEBUG);
  assert_int_equal(conf->ws_port,      8008);

  assert_int_equal(conf->size,      1);

  assert_string_equal(conn->name, "gw05.mor");
  assert_int_equal(conn->port, 5038);
  assert_string_equal(conn->host, "10.168.1.100");
  assert_string_equal(conn->username, "admin");
  assert_string_equal(conn->secret, "passw0rd");
}

static void config_file_with_multiple_hosts(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/multi_ami_host.yml");
  struct amiws_conn *conn;

  assert_int_equal(conf->size, 3);
  conn = conf->head;

  assert_string_equal(conn->name, "gw01.host");
  assert_int_equal(conn->port, 5038);
  assert_string_equal(conn->host, "10.168.1.100");
  assert_string_equal(conn->username, "admin");
  assert_string_equal(conn->secret, "pass001");
  assert_int_equal(conn->id, 1);

  conn = conn->next;
  assert_non_null(conn);
  assert_string_equal(conn->name, "gw02.host");
  assert_int_equal(conn->port, 5039);
  assert_string_equal(conn->host, "10.12.30.1");
  assert_string_equal(conn->username, "admin");
  assert_string_equal(conn->secret, "pass002");
  assert_int_equal(conn->id, 2);

  conn = conn->next;
  assert_non_null(conn);
  assert_string_equal(conn->name, "gw03.ast");
  assert_int_equal(conn->port, 6038);
  assert_string_equal(conn->host, "10.0.110.10");
  assert_string_equal(conn->username, "asterisk");
  assert_string_equal(conn->secret, "pass003");
  assert_int_equal(conn->id, 3);
}

static void config_file_test_default_vals(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/default_vals.yml");
  struct amiws_conn *conn = conf->head;
  assert_int_equal(conf->size, 1);
  assert_int_equal(conf->log_facility, LOG_SYSLOG);
  assert_int_equal(conf->log_level,    LOG_INFO);
  assert_int_equal(conf->ws_port,      8000);

  assert_int_equal(conn->port, 5038);
}

static void config_file_with_missing_password (void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/missing_password.yml");
  assert_null(conf);
}

static void config_file_with_missing_name (void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/missing_name.yml");
  assert_null(conf);
}

static void config_file_with_missing_host (void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/missing_host.yml");
  assert_null(conf);
}

static void config_file_with_missing_username (void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/missing_username.yml");
  assert_null(conf);
}

static void config_file_with_single_host_connection_address(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/single_ami_host.yml");
  struct amiws_conn *conn = conf->head;
  assert_string_equal(conn->address, "tcp://10.168.1.100:5038");
}

static void config_file_with_multiple_hosts_connection_address(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/multi_ami_host.yml");
  struct amiws_conn *conn;

  conn = conf->head;
  assert_string_equal(conn->address, "tcp://10.168.1.100:5038");
  conn = conn->next;
  assert_string_equal(conn->address, "tcp://10.12.30.1:5039");
  conn = conn->next;
  assert_string_equal(conn->address, "tcp://10.0.110.10:6038");
}

static void config_file_test_default_vals_connection_address(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/default_vals.yml");
  struct amiws_conn *conn = conf->head;
  assert_string_equal(conn->address, "tcp://10.168.1.100:5038");
}

static void config_file_web_root_set(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/single_ami_host.yml");
  assert_string_equal(conf->web_root, "/home/amiws/web_root");
}

static void config_file_test_default_vals_for_web_root(void **state)
{
  (void) *state;
  struct amiws_config *conf = read_conf("fixtures/default_vals.yml");
  assert_string_equal(conf->web_root, "./web_root");
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(config_file_does_not_exists),
    cmocka_unit_test(config_file_with_invalid_int_value),
    cmocka_unit_test(config_file_with_single_host),
    cmocka_unit_test(config_file_with_multiple_hosts),
    cmocka_unit_test(config_file_test_default_vals),
    cmocka_unit_test(config_file_with_missing_password),
    cmocka_unit_test(config_file_with_missing_name),
    cmocka_unit_test(config_file_with_missing_host),
    cmocka_unit_test(config_file_with_missing_username),
    cmocka_unit_test(config_file_with_single_host_connection_address),
    cmocka_unit_test(config_file_with_multiple_hosts_connection_address),
    cmocka_unit_test(config_file_test_default_vals_connection_address),
    cmocka_unit_test(config_file_web_root_set),
    cmocka_unit_test(config_file_test_default_vals_for_web_root),
  };
  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Configuration file", tests, NULL, NULL);
}
