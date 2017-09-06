#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "amiws.h"

static void packet_prompt_with_three_digits_version (void **state)
{
  (void) *state;
  const char pack[] = "Asterisk Call Manager/2.8.4\r\n";
  assert_int_equal(AMI_PROMPT, amipack_parser_detect (pack));
}

static void packet_prompt_with_two_digits_version (void **state)
{
  (void) *state;
  const char pack[] = "Asterisk Call Manager/1.1\r\n";
  assert_int_equal(AMI_PROMPT, amipack_parser_detect (pack));
}

static void detect_single_header_action_packet (void **state)
{
  (void) *state;
  const char pack[] = "Action: CoreStatus\r\n\r\n";
  assert_int_equal(AMI_ACTION, amipack_parser_detect (pack));
}

static void detect_single_header_event_packet (void **state)
{
  (void) *state;
  const char pack[] = "Event: FullyBooted\r\n\r\n";
  assert_int_equal(AMI_EVENT, amipack_parser_detect (pack));
}

static void detect_event_multyline_packet (void **state)
{
  (void) *state;
  const char pack[] = "Event: Hangup\r\n"
                      "Privilege: call,all\r\n"
                      "Channel: SIP/ipauthTp3BCHH7-00573401\r\n"
                      "Uniqueid: 1486254977.6071371\r\n"
                      "CallerIDNum: 18072280333\r\n"
                      "CallerIDName: <unknown>\r\n"
                      "ConnectedLineNum: 16478472022\r\n"
                      "ConnectedLineName: John Bar\r\n"
                      "Cause: 16\r\n"
                      "Cause-txt: Normal Clearing\r\n\r\n";
  assert_int_equal(AMI_EVENT, amipack_parser_detect (pack));
}

static void detect_event_multyline_packet_with_empty_values (void **state)
{
  (void) *state;
  const char pack[] = "Event: Newchannel\r\n"
                      "Privilege: call,all\r\n"
                      "Channel: SIP/ipauthTp3BCHH7-00573539\r\n"
                      "ChannelState: 0\r\n"
                      "ChannelStateDesc: Down\r\n"
                      "CallerIDNum: \r\n"
                      "CallerIDName: \r\n"
                      "AccountCode: 81\r\n"
                      "Exten: \r\n"
                      "Context: mor\r\n"
                      "Uniqueid: 1486256739.6071687\r\n\r\n";
  assert_int_equal(AMI_EVENT, amipack_parser_detect (pack));
}

static void detect_single_header_response_packet (void **state)
{
  (void) *state;
  const char pack[] = "Response: Success\r\n\r\n";
  assert_int_equal(AMI_RESPONSE, amipack_parser_detect (pack));
}

static void detect_multiheader_response_packet (void **state)
{
  (void) *state;
  const char pack[] = "Response: Success\r\n"
                      "Message: Authentication accepted.\r\n\r\n";
  assert_int_equal(AMI_RESPONSE, amipack_parser_detect (pack));
}

static void detect_command_response (void **state)
{
  (void) *state;
  const char pack[] = "Response: Follows\r\n"
    "ActionID: 12345\r\n"
    "Privilege: Command\r\n"
    "Channel              Location             State   Application(Data)             \n"
    "Local/5143607296@dia IVR_603@default:1    Up      AppDial((Outgoing Line))      \n"
    "Local/5146020115@dia 5146020115@dial-foll Ring    Dial(SIP/5146020115@drspa.ntek\n"
    "1754093 calls processed\n"
    "--END COMMAND--\r\n\r\n";
  assert_int_equal(AMI_RESPCMD, amipack_parser_detect (pack));
}

static void detect_command_response_with_output_header (void **state)
{
  (void) *state;
  const char pack[] = "Response: Follows\r\n"
  "Message: Command output follows\r\n"
  "Output: Name/username             Host                                    Dyn Forcerport Comedia    ACL Port     Status      Description                      \r\n"
  "8888/8888                 (Unspecified)                            D  Auto (No)  No             0        Unmonitored                                  \n"
  "1 sip peers [Monitored: 0 online, 0 offline Unmonitored: 0 online, 1 offline]\n"
  "--END COMMAND--\r\n\r\n";
  assert_int_equal(AMI_RESPCMD, amipack_parser_detect (pack));
}

static void detect_queue_no_members_no_callers (void **state)
{
  (void) *state;
  const char pack[] = "NEXUS-223_sales has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:3, C:40, A:8, SL:2.7% within 1230s\r\n"
  "   No Members\r\n"
  "   No Callers\r\n\r\n";
  assert_int_equal(AMI_QUEUE, amipack_parser_detect (pack));
}

static void detect_queue_two_members_no_callers (void **state)
{
  (void) *state;
  const char pack[] = "janet_373 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:0, C:0, A:0, SL:0.0% within 0s\r\n"
  "   Members: \r\n"
  "      228@proforce.modulis.clusterpbx.ca (Local/228@from-queue/n from Local/228@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
  "      5147308585@proforce.modulis.clusterpbx.c (Local/5147308585@from-queue/n from Local/5147308585@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
  "   No Callers\r\n\r\n";
  assert_int_equal(AMI_QUEUE, amipack_parser_detect (pack));
}

static void detect_queue_no_members_three_callers (void **state)
{
  (void) *state;
  const char pack[] = "janet_373 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:0, C:0, A:0, SL:0.0% within 0s\r\n"
  "   No Members\r\n"
  "   Callers: \r\n"
  "      1. SIP/router01-0001e28a (wait: 5:06, prio: 0)\r\n"
  "      2. SIP/router01-0001e32a (wait: 1:04, prio: 0)\r\n"
  "      3. SIP/router01-0001e333 (wait: 0:41, prio: 0)\r\n\r\n";
  assert_int_equal(AMI_QUEUE, amipack_parser_detect (pack));
}

static void detect_queue_two_members_three_callers (void **state)
{
  (void) *state;
  const char pack[] = "janet_373 has 0 calls (max unlimited) in 'ringall' strategy (0s holdtime, 0s talktime), W:0, C:0, A:0, SL:0.0% within 0s\r\n"
  "   Members: \r\n"
  "      228@proforce.modulis.clusterpbx.ca (Local/228@from-queue/n from Local/228@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
  "      5147308585@proforce.modulis.clusterpbx.c (Local/5147308585@from-queue/n from Local/5147308585@from-queue/n) (ringinuse disabled) (realtime) (paused) (Not in use) has taken no calls yet\r\n"
  "   Callers: \r\n"
  "      1. SIP/router01-0001e28a (wait: 5:06, prio: 0)\r\n"
  "      2. SIP/router01-0001e32a (wait: 1:04, prio: 0)\r\n"
  "      3. SIP/router01-0001e333 (wait: 0:41, prio: 0)\r\n\r\n";
  assert_int_equal(AMI_QUEUE, amipack_parser_detect (pack));
}

static void unknown_invalid_packet_1 (void **state)
{
  (void) *state;
  const char pack[] = "this is invalid packet\r\n\r\n";
  assert_int_equal(AMI_UNKNOWN, amipack_parser_detect (pack));
}

static void unknown_invalid_packet_2 (void **state)
{
  (void) *state;
  const char pack[] = "this is invalid packet";
  assert_int_equal(AMI_UNKNOWN, amipack_parser_detect (pack));
}

static void unknown_invalid_packet_with_multiheaders (void **state)
{
  (void) *state;
  const char pack[] = "Event: Hangup\r\n"
                      "Privilege: call,all\r\n"
                      "Channel: SIP/ipauthTp3BCHH7-00573401\r\n"
                      "Uniqueid: 1486254977.6071371\r\n"
                      "CallerIDNum: 18072280333\r\n"
                      "CallerIDName: <unknown>\r\n"
                      "ConnectedLineNum: 16478472022\r\n";
  assert_int_equal(AMI_UNKNOWN, amipack_parser_detect (pack));
}


int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(packet_prompt_with_three_digits_version),
    cmocka_unit_test(packet_prompt_with_two_digits_version),
    cmocka_unit_test(detect_single_header_action_packet),
    cmocka_unit_test(detect_single_header_event_packet),
    cmocka_unit_test(detect_event_multyline_packet),
    cmocka_unit_test(detect_event_multyline_packet_with_empty_values),
    cmocka_unit_test(detect_single_header_response_packet),
    cmocka_unit_test(detect_multiheader_response_packet),
    cmocka_unit_test(detect_command_response),
    cmocka_unit_test(detect_command_response_with_output_header),
    cmocka_unit_test(detect_queue_no_members_no_callers),
    cmocka_unit_test(detect_queue_two_members_no_callers),
    cmocka_unit_test(detect_queue_no_members_three_callers),
    cmocka_unit_test(detect_queue_two_members_three_callers),
    cmocka_unit_test(unknown_invalid_packet_1),
    cmocka_unit_test(unknown_invalid_packet_2),
    cmocka_unit_test(unknown_invalid_packet_with_multiheaders),
  };
  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Detect packet type", tests, NULL, NULL);
}
