LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/tap-driver.sh

#TESTS = config_test pack_scan amipack_create_test amipack_parse_test amipack_parse_queue_test
TESTS = test_config test_pack_scan   \
        test_create_packet           \
        test_pack_type_detect        \
        test_parser_prompt           \
        test_parser_message          \
        test_parser_command          \
        test_parser_queue

#check_PROGRAMS = config_test pack_scan amipack_create_test amipack_parse_test amipack_parse_queue_test
check_PROGRAMS = test_config test_pack_scan   \
                 test_create_packet           \
                 test_pack_type_detect        \
                 test_parser_prompt           \
                 test_parser_message          \
                 test_parser_command          \
                 test_parser_queue

test_config_SOURCE = test_config.c
test_config_CFLAGS = -I$(top_builddir)/src/lib
test_config_LDADD  = -L$(top_builddir)/src/lib -lamiws -lamipack -lmongoose -lfrozen -lcmocka

test_pack_scan_SOURCE = test_pack_scan.c
test_pack_scan_CFLAGS = -I$(top_builddir)/src/lib
test_pack_scan_LDADD  = -L$(top_builddir)/src/lib -lamiws -lamipack -lmongoose -lfrozen -lcmocka

test_create_packet_SOURCES = test_create_packet.c
test_create_packet_CFLAGS  = -I$(top_builddir)/src/lib
test_create_packet_LDADD   = -L$(top_builddir)/src/lib -lamipack -lcmocka

test_pack_type_detect_SOURCE = test_pack_type_detect.c
test_pack_type_detect_CFLAGS = -I$(top_builddir)/src/lib
test_pack_type_detect_LDADD  = -L$(top_builddir)/src/lib -lamiws -lamipack -lmongoose -lfrozen -lcmocka

test_parser_prompt_SOURCES = test_parser_prompt.c
test_parser_prompt_CFLAGS  = -I$(top_builddir)/src/lib
test_parser_prompt_LDADD   = -L$(top_builddir)/src/lib -lamipack -lcmocka

test_parser_message_SOURCES = test_parser_message.c
test_parser_message_CFLAGS  = -I$(top_builddir)/src/lib
test_parser_message_LDADD   = -L$(top_builddir)/src/lib -lamipack -lcmocka

test_parser_command_SOURCES = test_parser_command.c
test_parser_command_CFLAGS  = -I$(top_builddir)/src/lib
test_parser_command_LDADD   = -L$(top_builddir)/src/lib -lamipack -lcmocka

test_parser_queue_SOURCES = test_parser_queue.c
test_parser_queue_CFLAGS  = -I$(top_builddir)/src/lib
test_parser_queue_LDADD   = -L$(top_builddir)/src/lib -lamiws -lamipack -lmongoose -lfrozen -lcmocka

