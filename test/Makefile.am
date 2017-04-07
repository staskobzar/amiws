LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/tap-driver.sh

TESTS = 01_test_ami.t

check_PROGRAMS = mytest

mytest_SOURCE = mytest.c $(top_builddir)/src/amiws.h
mytest_CFLAGS = -I$(top_builddir)/src

