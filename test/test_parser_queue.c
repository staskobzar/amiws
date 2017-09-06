#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>

#include "amipack.h"

static void parse_pack_queue_type (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "NOTIXIA_695 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);

  amipack_queue_destroy (queue);
}

static void parse_pack_queue_name (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "NEXUS-223_sales has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_string_equal(queue->name, "NEXUS-223_sales");
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_calls (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "223_sales has 8 calls (max 32) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_string_equal(queue->name, "223_sales");
  assert_int_equal (queue->calls, 8);
  assert_int_equal (queue->maxlen, 32);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_maxlen_unlimited (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "223_sales has 17 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_int_equal (queue->calls, 17);
  assert_int_equal (queue->maxlen, 0);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_strategy (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "timeOut_1251 has 5 calls (max unlimited) in 'ringall' strategy (4s holdtime, 327s talktime), W:0, C:24, A:11, SL:0.0% within 0s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_string_equal(queue->strategy, "ringall");
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_holdtime (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "timeOut_1251 has 0 calls (max unlimited) in 'fewestcalls' strategy (4s holdtime, 327s talktime), W:0, C:24, A:11, SL:0.0% within 0s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_string_equal(queue->strategy, "fewestcalls");
  assert_int_equal (queue->holdtime, 4);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_talktime (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "timeOut_1251 has 0 calls (max unlimited) in 'fewestcalls' strategy (0s holdtime, 223s talktime), W:0, C:24, A:11, SL:0.0% within 0s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_int_equal (queue->talktime, 223);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_calls_stats (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "timeOut_1251 has 0 calls (max unlimited) in 'fewestcalls' strategy (4s holdtime, 327s talktime), W:2, C:24, A:11, SL:0.0% within 0s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_int_equal (queue->weight, 2);
  assert_int_equal (queue->callscompleted, 24);
  assert_int_equal (queue->callsabandoned, 11);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_service_level (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "Queue_1259 has 0 calls (max 8) in 'ringall' strategy (3s holdtime, 180s talktime), W:0, C:815, A:17, SL:4.00546% within 58s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_string_equal(queue->sl, "4.00546%");
  assert_int_equal (queue->sl_sec, 58);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_with_no_members_and_no_callers (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "NOTIXIA_695 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_int_equal (queue->members_size, 0);
  assert_int_equal (queue->callers_size, 0);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_with_two_members_and_no_callers (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "janet_373 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:0, C:0, A:0, SL:0.0% within 0s\r\n"
      "   Members: \r\n"
      "      228@proforce.modulis.clusterpbx.ca (Local/228@from-queue/n from Local/228@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
      "      5147308585@proforce.modulis.clusterpbx.c (Local/5147308585@from-queue/n from Local/5147308585@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
      "   No Callers\r\n"
      "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_int_equal (queue->members_size, 2);
  assert_int_equal (queue->callers_size, 0);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_with_no_members_and_three_callers (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "janet_373 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:0, C:0, A:0, SL:0.0% within 0s\r\n"
      "   No Members\r\n"
      "   Callers: \r\n"
      "      1. SIP/router01-0001e28a (wait: 5:06, prio: 0)\r\n"
      "      2. SIP/router01-0001e32a (wait: 1:04, prio: 0)\r\n"
      "      3. SIP/router01-0001e333 (wait: 0:41, prio: 0)\r\n"
      "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_int_equal (queue->members_size, 0);
  assert_int_equal (queue->callers_size, 3);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_with_two_members_and_three_callers (void **state)
{
  (void)*state;
  // pending
  AMIQueue *queue;
  const char inpack[] = "janet_373 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:0, C:0, A:0, SL:0.0% within 0s\r\n"
      "   Members: \r\n"
      "      228@proforce.modulis.clusterpbx.ca (Local/228@from-queue/n from Local/228@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
      "      5147308585@proforce.modulis.clusterpbx.c (Local/5147308585@from-queue/n from Local/5147308585@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
      "   Callers: \r\n"
      "      1. SIP/router01-0001e28a (wait: 5:06, prio: 0)\r\n"
      "      2. SIP/router01-0001e32a (wait: 1:04, prio: 0)\r\n"
      "      3. SIP/router01-0001e333 (wait: 0:41, prio: 0)\r\n"
      "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_non_null(queue);
  assert_int_equal (queue->members_size, 2);
  assert_int_equal (queue->callers_size, 3);
  amipack_queue_destroy (queue);
}

static void parse_pack_queue_invalid (void **state)
{
  (void)*state;
  AMIQueue *queue;
  const char inpack[] = "NOTIXIA_695 has 0 calls (max unlimited) in 'ringall' strategy\r\n"
  "\r\n";

  queue = amipack_parser_queue (inpack);
  assert_null(queue);

}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (parse_pack_queue_type),
    cmocka_unit_test (parse_pack_queue_name),
    cmocka_unit_test (parse_pack_queue_calls),
    cmocka_unit_test (parse_pack_queue_maxlen_unlimited),
    cmocka_unit_test (parse_pack_queue_strategy),
    cmocka_unit_test (parse_pack_queue_holdtime),
    cmocka_unit_test (parse_pack_queue_talktime),
    cmocka_unit_test (parse_pack_queue_calls_stats),
    cmocka_unit_test (parse_pack_queue_service_level),
    cmocka_unit_test (parse_pack_queue_with_no_members_and_no_callers),
    cmocka_unit_test (parse_pack_queue_with_two_members_and_no_callers),
    cmocka_unit_test (parse_pack_queue_with_no_members_and_three_callers),
    cmocka_unit_test (parse_pack_queue_with_two_members_and_three_callers),
    cmocka_unit_test (parse_pack_queue_invalid),
  };

  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Parse AMI queues response tests.", tests, NULL, NULL);
}
