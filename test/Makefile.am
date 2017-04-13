LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/tap-driver.sh

TESTS = config_test

check_PROGRAMS = config_test

config_test_SOURCE=config_test.c
config_test_CFLAGS= -I$(top_builddir)/src/lib
config_test_LDADD= -L$(top_builddir)/src/lib -lamiws -lmongoose -lfrozen -lcmocka

