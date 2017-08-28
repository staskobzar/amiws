#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>

#include "amipack.h"

static void parse_pack_queue_type (void **state)
{
  (void)*state;
  AMIPacket *pack;
  const char inpack[] = "NOTIXIA_695 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  //  "   Members:\r\n"
  //  "      101@witlaken.modulis.clusterpbx.ca (Local/101@from-queue/n from Local/101@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
  //  "   No Callers\r\n"
  "\r\n";

  pack = amiparse_pack (inpack);
  assert_int_equal (AMI_QUEUES, pack->type);

  amipack_destroy (pack);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (parse_pack_queue_type),
  };

  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Parse AMI queues response tests.", tests, NULL, NULL);
}
