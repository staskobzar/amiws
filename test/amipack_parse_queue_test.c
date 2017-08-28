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
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  pack = amiparse_pack (inpack);
  assert_int_equal (AMI_QUEUES, pack->type);

  amipack_destroy (pack);
}

static void parse_pack_queue_with_no_members_and_no_callers (void **state)
{
  (void)*state;
  AMIPacket *pack;
  const char inpack[] = "NOTIXIA_695 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  pack = amiparse_pack (inpack);
  assert_int_equal (AMI_QUEUES, pack->type);
  assert_non_null(pack->queue);
  assert_int_equal (pack->queue->members_size, 0);
  assert_int_equal (pack->queue->callers_size, 0);
  amipack_destroy (pack);
}

static void parse_pack_queue_with_two_members_and_no_callers (void **state)
{
  (void)*state;
  // pending
}

static void parse_pack_queue_with_no_members_and_three_callers (void **state)
{
  (void)*state;
  // pending
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (parse_pack_queue_type),
    cmocka_unit_test (parse_pack_queue_with_no_members_and_no_callers),
  };

  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Parse AMI queues response tests.", tests, NULL, NULL);
}
