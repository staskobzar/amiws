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
  assert_int_equal(conf->log_facility, LOG_LOCAL4);
  assert_int_equal(conf->log_level,    LOG_DEBUG);
  assert_int_equal(conf->ws_port,      8008);

  assert_int_equal(conf->size,      1);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(config_file_does_not_exists),
    cmocka_unit_test(config_file_with_single_host),
    cmocka_unit_test(config_file_with_invalid_int_value),
  };
  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Configuration file", tests, NULL, NULL);
}
