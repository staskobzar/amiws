LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/tap-driver.sh

TESTS = config_test pack_scan amipack_create_test amipack_parse_test amipack_parse_queue_test

check_PROGRAMS = config_test pack_scan amipack_create_test amipack_parse_test amipack_parse_queue_test

config_test_SOURCE = config_test.c
config_test_CFLAGS = -I$(top_builddir)/src/lib
config_test_LDADD  = -L$(top_builddir)/src/lib -lamiws -lamipack -lmongoose -lfrozen -lcmocka

pack_scan_SOURCE = pack_scan.c
pack_scan_CFLAGS = -I$(top_builddir)/src/lib
pack_scan_LDADD  = -L$(top_builddir)/src/lib -lamiws -lamipack -lmongoose -lfrozen -lcmocka

amipack_create_test_SOURCES = amipack_create_test.c
amipack_create_test_CFLAGS  = -I$(top_builddir)/src/lib
amipack_create_test_LDADD   = -L$(top_builddir)/src/lib -lamipack -lcmocka

amipack_parse_test_SOURCES = amipack_parse_test.c
amipack_parse_test_CFLAGS  = -I$(top_builddir)/src/lib
amipack_parse_test_LDADD   = -L$(top_builddir)/src/lib -lamipack -lcmocka

amipack_parse_queue_test_SOURCES = amipack_parse_queue_test.c
amipack_parse_queue_test_CFLAGS  = -I$(top_builddir)/src/lib
amipack_parse_queue_test_LDADD   = -L$(top_builddir)/src/lib -lamipack -lcmocka

