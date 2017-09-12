#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "amipack.h"

static void parse_command_show_hints (void **state)
{
  (void)*state;
  AMIPacket *pack;
  size_t len;
  char *hv; // header value
  enum pack_type type;

  const char str_pack[] = "Response: Follows\r\n"
                          "Privilege: Command\r\n"
                          "There are no registered dialplan hints\n"
                          "--END COMMAND--\r\n\r\n";
  type = amipack_parser_detect(str_pack);
  assert_int_equal (AMI_RESPCMD, type);
  pack = amipack_parser_command (str_pack);
  len = amiheader_find(pack, "Privilege", &hv);
  assert_string_equal(hv, "Command");

  amipack_destroy (pack);
}

static void parse_pack_command_output (void **state)
{
  (void)*state;
  AMIPacket *pack;
  size_t len;
  char *hv; // header value

  const char str_pack[] = "Response: Follows\r\n"
                          "ActionID: 12345\r\n"
                          "Privilege: Command\r\n"
                          "Channel              Location             State   Application(Data)             \n"
                          "Local/5143607296@dia IVR_603@default:1    Up      AppDial((Outgoing Line))      \n"
                          "Local/5146020115@dia 5146020115@dial-foll Ring    Dial(SIP/5146020115@drspa.ntek\n"
                          "1754093 calls processed\n"
                          "--END COMMAND--\r\n\r\n";

  const char output_cmp[] = "Channel              Location             State   Application(Data)             \n"
                          "Local/5143607296@dia IVR_603@default:1    Up      AppDial((Outgoing Line))      \n"
                          "Local/5146020115@dia 5146020115@dial-foll Ring    Dial(SIP/5146020115@drspa.ntek\n"
                          "1754093 calls processed\n";

  pack = amipack_parser_command (str_pack);

  len = amiheader_find(pack, "Privilege", &hv);
  assert_string_equal(hv, "Command");

  len = amiheader_find(pack, "ActionID", &hv);
  assert_string_equal(hv, "12345");

  len = amiheader_find(pack, "Output", &hv);
  assert_string_equal(hv, output_cmp);

  amipack_destroy (pack);
}

// Command output compatibale with AMI v2
// Command output is a value of the header "Output: "
static void parse_pack_command_output_v2 (void **state)
{
  (void)*state;
  AMIPacket *pack;
  size_t len;
  char *hv; // header value

  const char str_pack[] = "Response: Follows\r\n"
                          "Message: Command output follows\r\n"
                          "Output: Name/username             Host                                    Dyn Forcerport Comedia    ACL Port     Status      Description                      \r\n"
                          "8888/8888                 (Unspecified)                            D  Auto (No)  No             0        Unmonitored                                  \n"
                          "1 sip peers [Monitored: 0 online, 0 offline Unmonitored: 0 online, 1 offline]\n"
                          "--END COMMAND--\r\n\r\n";

  const char output_cmp[] = "Name/username             Host                                    Dyn Forcerport Comedia    ACL Port     Status      Description                      \r\n"
                          "8888/8888                 (Unspecified)                            D  Auto (No)  No             0        Unmonitored                                  \n"
                          "1 sip peers [Monitored: 0 online, 0 offline Unmonitored: 0 online, 1 offline]\n";

  pack = amipack_parser_command (str_pack);

  len = amiheader_find(pack, "Message", &hv);
  assert_string_equal(hv, "Command output follows");

  len = amiheader_find(pack, "Output", &hv);
  assert_string_equal(hv, output_cmp);

  amipack_destroy (pack);
}

int main(void)
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test (parse_command_show_hints),
    cmocka_unit_test (parse_pack_command_output),
    cmocka_unit_test (parse_pack_command_output_v2),
  };

  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Parse AMI command output.", tests, NULL, NULL);
}
