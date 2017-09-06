#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>

#include "amipack.h"

static void parse_prompt_header_11 (void **state)
{
  (void)*state;
  const char *str = "Asterisk Call Manager/1.1\r\n";
  AMIVer ver;
  int ret = amipack_parser_prompt (str, &ver);
  assert_int_equal (1, ret);
  assert_int_equal (1, ver.major);
  assert_int_equal (1, ver.minor);
  assert_int_equal (0, ver.patch);
}

static void parse_prompt_header_284 (void **state)
{
  (void)*state;
  const char *str = "Asterisk Call Manager/2.8.4\r\n";
  AMIVer ver;
  int ret = amipack_parser_prompt (str, &ver);
  assert_int_equal (1, ret);
  assert_int_equal (2, ver.major);
  assert_int_equal (8, ver.minor);
  assert_int_equal (4, ver.patch);
}

static void parse_not_prompt_pack (void **state)
{
  (void)*state;
  const char *str = "Response: Success\r\n";
  AMIVer ver;
  int ret = amipack_parser_prompt (str, &ver);
  assert_int_equal (0, ret);
  assert_int_equal (0, ver.major);
  assert_int_equal (0, ver.minor);
  assert_int_equal (0, ver.patch);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (parse_prompt_header_11),
    cmocka_unit_test (parse_prompt_header_284),
    cmocka_unit_test (parse_not_prompt_pack),
  };

  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Parse AMI package tests.", tests, NULL, NULL);
}
