#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([amiws], [1.0.0], [staskobzar@gmail.com])
AM_INIT_AUTOMAKE([foreign parallel-tests color-tests -Wall -Werror])

AC_PROG_RANLIB

AC_CONFIG_SRCDIR([src/amiws.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC

# Checks for libraries.
#AC_CHECK_LIB([amip], [amipack_init], [],
#             [ AC_MSG_ERROR([AMI library "amip" not found.
#        >> Install it from "https://github.com/staskobzar/libamip" and retry.]) ])

AC_CHECK_LIB([yaml], [yaml_parser_initialize], [],
             [ AC_MSG_ERROR([YAML library not found.
        >> Install libyaml and retry.]) ])

# Checks for header files.
#AC_CHECK_HEADER([amip.h], [],
#             [ AC_MSG_ERROR([AMI library header "amip.h" not found.
#        >> Install it from "https://github.com/staskobzar/libamip" and retry.]) ])

# Checks for syslog header files.
AC_CHECK_HEADER([syslog.h], [],
             [ AC_MSG_ERROR([syslog library header "syslog.h" not found.]) ])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_PROG_AR
AM_PROG_CC_C_O
AC_CONFIG_FILES([Makefile
                 doc/Doxyfile
                 doc/Makefile
                 src/Makefile
                 src/lib/Makefile
                 test/Makefile])

# Enable Test Anything Protocol
AC_REQUIRE_AUX_FILE([tap-driver.sh])
AC_PROG_AWK

AC_OUTPUT
