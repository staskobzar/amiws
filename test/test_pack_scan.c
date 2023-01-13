#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "amiws.h"

static int exiting = 0;

int isExiting() {
    return exiting;
}

void setExiting() {
    exiting = 1;
}


static void scan_ami_packet_from_tcp (void **state)
{
  (void) *state;
  char *pack =  "Event: PeerStatus\r\n"
                "Privilege: system,all\r\n"
                "ChannelType: SIP\r\n"
                "Peer: SIP/1155\r\n"
                "PeerStatus: Registered\r\n"
                "Address: 24.37.93.51:5060\r\n\r\n";
  assert_int_equal(scan_amipack(pack, strlen(pack)), 129);
}

static void scan_incomplete_ami_packet (void **state)
{
  (void) *state;
  char *pack =  "Event: PeerStatus\r\n"
                "Privilege: system,all\r\n"
                "ChannelType: SIP\r\n"
                "Peer: SIP/115";
  assert_int_equal(scan_amipack(pack, strlen(pack)), 0);
}

static void scan_pack_with_zero_len (void **state)
{
  (void) *state;
  char *pack =  "";
  assert_int_equal(scan_amipack(pack, strlen(pack)), 0);
}

static void scan_pack_termination_only (void **state)
{
  (void) *state;
  char *pack =  "\r\n\r\n";
  assert_int_equal(scan_amipack(pack, strlen(pack)), 4);
}

static void scan_pack_from_stream (void **state)
{
  (void) *state;
  char *pack =  "Event: PeerStatus\r\n"
                "Privilege: system,all\r\n"
                "PeerStatus: Registered\r\n"
                "Address: 24.37.93.51:5060\r\n\r\n"
                "Event: State\r\n"
                "State: OK";
  assert_int_equal(scan_amipack(pack, strlen(pack)), 95);
}

static void scan_pack_multy_packets (void **state)
{
  (void) *state;
  char *pack =  "Event: PeerStatus\r\n"
                "Privilege: system,all\r\n"
                "PeerStatus: Registered\r\n\r\n"
                "Event: State\r\n"
                "State: OK\r\n"
                "Address: 24.37.93.51:5060\r\n\r\n";
  assert_int_equal(scan_amipack(pack, strlen(pack)), 68);
}

int main(int argc, const char *argv[])
{
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(scan_ami_packet_from_tcp),
    cmocka_unit_test(scan_incomplete_ami_packet),
    cmocka_unit_test(scan_pack_with_zero_len),
    cmocka_unit_test(scan_pack_termination_only),
    cmocka_unit_test(scan_pack_from_stream),
    cmocka_unit_test(scan_pack_multy_packets),
  };
  cmocka_set_message_output(CM_OUTPUT_TAP);

  return cmocka_run_group_tests_name("Packet scan", tests, NULL, NULL);
}
