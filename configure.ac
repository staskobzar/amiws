#
# amiws -- Library with functions for read/create AMI packets
# Copyright (C) 2017, Stas Kobzar <staskobzar@modulis.ca>
#
# This file is part of amiws.
#
# amiws is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# amiws is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with amiws.  If not, see <http://www.gnu.org/licenses/>.
#

AC_PREREQ([2.63])
AC_INIT([amiws], [2.2.1], [staskobzar@gmail.com])
AM_INIT_AUTOMAKE([foreign parallel-tests color-tests -Wall -Werror])

AC_PROG_RANLIB

AC_CONFIG_SRCDIR([src/amiws.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_CC_STDC

AC_ARG_WITH([ssl],
    AS_HELP_STRING([--with-ssl], [With SSL support.]),
    [with_ssl=yes])

# Checks for libraries.
AC_CHECK_LIB([yaml], [yaml_parser_initialize], [],
             [ AC_MSG_ERROR([YAML library not found.
        >> Install libyaml and retry.]) ])

AS_IF([test "x$with_ssl" = "xyes"], [
  AC_SEARCH_LIBS([CRYPTO_new_ex_data], [eay32 crypto], [], [
                  AC_MSG_ERROR([crypto library not found])
                  ])
  AC_SEARCH_LIBS([SSL_library_init], [ssleay32 ssl], [], [
                  AC_MSG_ERROR([openssl library not found])
                  ])
  AC_SUBST([AM_CPPFLAGS], [-DMG_ENABLE_SSL])
])

# Checks for header files.

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

