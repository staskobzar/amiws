#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include "amipack.h"

static int setup_pack(void **state)
{
  *state = (AMIPacket *) amipack_init ();
  return 0;
}

static int teardown_pack(void **state)
{
  amipack_destroy (*state);
  return 0;
}

static void create_pack_with_no_header (void **state)
{
  char *pack_str;
  AMIPacket *pack = *state;

  assert_int_equal (pack->size, 0);
  assert_int_equal ( amipack_to_str(pack, &pack_str), 0 );
}

static void create_pack_with_single_header (void **state)
{
  char *pstr;
  AMIPacket *pack = *state;
  size_t plen;

  amipack_type(pack, AMI_ACTION);
  amipack_append_action(pack, strdup("CoreStatus"), 10);

  assert_int_equal(pack->size, 1);

  plen = amipack_to_str(pack, &pstr);
  assert_memory_equal(pstr, "Action: CoreStatus\r\n\r\n", plen);

  assert_int_equal(plen, amipack_length(pack));

  free(pstr);
}

static void create_pack_with_two_headers (void **state)
{
  char *pack_str;
  size_t len;
  AMIPacket *pack = *state;

  amipack_type(pack, AMI_ACTION);
  amipack_append_action(pack, strdup("Command"), 7);
  amipack_append (pack, strdup("Command"), 7, strdup("core show uptime"), 16);

  assert_int_equal (pack->size, 2);

  len = amipack_to_str (pack, &pack_str);
  assert_memory_equal (pack_str,
      "Action: Command\r\nCommand: core show uptime\r\n\r\n",
      len);

  assert_int_equal(len, amipack_length (pack));
  free(pack_str);
}

static void create_pack_with_three_headers (void **state)
{
  char *pack_str;
  size_t len;
  AMIPacket *pack = *state;

  amipack_type(pack, AMI_ACTION);
  amipack_append_action(pack, strdup("ExtensionState"), 14);
  amipack_append (pack, strdup("Exten"), 5, strdup("5555"), 4);
  amipack_append (pack, strdup("Context"), 7, strdup("inbound-local"), 13);

  assert_int_equal (pack->size, 3);

  len = amipack_to_str (pack, &pack_str);
  assert_memory_equal (pack_str,
      "Action: ExtensionState\r\nExten: 5555\r\nContext: inbound-local\r\n\r\n",
      len);

  assert_int_equal(len, amipack_length (pack));
  free(pack_str);
}

static void create_pack_with_multi_headers (void **state)
{
  size_t len;
  const char *pack_result = "Action: Redirect\r\n"
                            "Channel: SIP/5558877449-C-00006cf\r\n"
                            "ExtraChannel: SIP/258-C-000069a\r\n"
                            "Context: outbound-local\r\n"
                            "ExtraContext: extens-internal\r\n"
                            "Priority: 1\r\n"
                            "ExtraPriority: 1\r\n\r\n";
  char *pack_str;
  AMIPacket *pack = *state;

  amipack_type(pack, AMI_ACTION);
  amipack_append_action (pack, strdup("Redirect"), 8);
  amipack_append (pack, strdup("Channel"), 7, strdup("SIP/5558877449-C-00006cf"), 24);
  amipack_append (pack, strdup("ExtraChannel"), 12, strdup("SIP/258-C-000069a"), 17);
  amipack_append (pack, strdup("Context"), 7, strdup("outbound-local"), 14);
  amipack_append (pack, strdup("ExtraContext"), 12, strdup("extens-internal"), 15);
  amipack_append (pack, strdup("Priority"), 8, strdup("1"), 1);
  amipack_append (pack, strdup("ExtraPriority"), 13, strdup("1"), 1);

  assert_int_equal (pack->size, 7);

  len = amipack_to_str (pack, &pack_str);

  assert_memory_equal (pack_str, pack_result, len);

  assert_int_equal(len, amipack_length (pack));
  free(pack_str);
}

static void create_pack_with_empty_header (void **state)
{
  char *pack_str;
  size_t len;
  AMIPacket *pack = *state;

  amipack_type(pack, AMI_EVENT);

  amipack_append (pack, strdup("Event"), 5, strdup("Newchannel"), 10);
  amipack_append (pack, strdup("ChannelState"), 12, strdup("0"), 1);
  amipack_append (pack, strdup("CallerIDNum"), 11, strdup(""), 0);
  amipack_append (pack, strdup("CallerIDName"),12, NULL, 0);
  amipack_append (pack, strdup("Exten1"), 6, strdup(""), 0);
  amipack_append (pack, strdup("Context"), 7, strdup("mor"), 3);

  assert_int_equal (pack->size, 6);
  len = amipack_to_str (pack, &pack_str);
  assert_memory_equal (pack_str,
      "Event: Newchannel\r\nChannelState: 0\r\nCallerIDNum: \r\nCallerIDName: \r\nExten1: \r\nContext: mor\r\n\r\n",
      len);
  assert_int_equal(len, amipack_length (pack));
  free(pack_str);
}

static void create_pack_with_empty_last_header (void **state)
{
  char *pack_str;
  size_t len;
  AMIPacket *pack = *state;

  amipack_type(pack, AMI_EVENT);

  amipack_append (pack, strdup("Event"), 5, strdup("Newchannel"), 10);
  amipack_append (pack, strdup("ChannelState"), 12, strdup("0"), 1);
  amipack_append (pack, strdup("CallerIDNum"), 11, strdup(""), 0);

  assert_int_equal (pack->size, 3);
  len = amipack_to_str (pack, &pack_str);
  assert_memory_equal (pack_str,
      "Event: Newchannel\r\nChannelState: 0\r\nCallerIDNum: \r\n\r\n",
      len);
  assert_int_equal(len, amipack_length (pack));
  free (pack_str);
}

static void pack_find_headers (void **state)
{
  AMIPacket *pack = *state;
  char *hdr_val; // header value
  size_t len;

  amipack_type(pack, AMI_EVENT);
  amipack_append (pack, strdup("Event"), 5, strdup("Hangup"), 6);
  amipack_append (pack, strdup("Privilege"), 9, strdup("dialplan,all"), 12);
  amipack_append (pack, strdup("Channel"), 7, strdup("PJSIP/kermit-00000001"), 21);
  amipack_append (pack, strdup("Uniqueid"), 8, strdup("asterisk-1368479157.1"), 21);
  amipack_append (pack, strdup("ChannelState"), 12, strdup("6"), 1);
  amipack_append (pack, strdup("ChannelStateDesc"), 16, strdup("Up"), 2);
  amipack_append (pack, strdup("Cause"), 5, strdup("16"), 2);
  amipack_append (pack, strdup("Cause-txt"), 9, strdup("Normal Clearing"), 15);

  // found header value
  len = amiheader_find(pack, "Cause", &hdr_val);
  assert_memory_equal(hdr_val, "16", len);

  len = amiheader_find(pack, "Uniqueid", &hdr_val);
  assert_memory_equal(hdr_val, "asterisk-1368479157.1", len);

  // case insencetive
  len = amiheader_find(pack, "chaNNelsTateDesc", &hdr_val);
  assert_memory_equal(hdr_val, "Up", len);

  // header value not found
  len = amiheader_find(pack, "TheHeaderThatDoesNotExists", &hdr_val);
  assert_int_equal(-1, len);

  len = amiheader_find(pack, "Grivileje", &hdr_val);
  assert_int_equal(-1, len);

  assert_int_equal (pack->size, 8);
  assert_int_equal (pack->type, AMI_EVENT);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test_setup_teardown (create_pack_with_no_header, setup_pack, teardown_pack),
    cmocka_unit_test_setup_teardown (create_pack_with_single_header, setup_pack, teardown_pack),
    cmocka_unit_test_setup_teardown (create_pack_with_two_headers, setup_pack, teardown_pack),
    cmocka_unit_test_setup_teardown (create_pack_with_three_headers, setup_pack, teardown_pack),
    cmocka_unit_test_setup_teardown (create_pack_with_multi_headers, setup_pack, teardown_pack),
    cmocka_unit_test_setup_teardown (create_pack_with_empty_header, setup_pack, teardown_pack),
    cmocka_unit_test_setup_teardown (create_pack_with_empty_last_header, setup_pack, teardown_pack),
    cmocka_unit_test_setup_teardown (pack_find_headers, setup_pack, teardown_pack),
  };

  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Create AMI package tests.", tests, NULL, NULL);
}
