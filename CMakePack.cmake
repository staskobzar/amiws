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
set (CPACK_GENERATOR "RPM")
set (CPACK_PACKAGE_VERSION ${amiws_VERSION})
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Asterisk Management Interface (AMI) to Web-socket proxy")

set (CPACK_DEBIAN_PACKAGE_MAINTAINER "Stas Kobzar <staskobzar@gmail.com>")
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libyaml")

set (CPACK_RPM_PACKAGE_VENDOR "Stas Kobzar <staskobzar@gmail.com>")
set (CPACK_RPM_PACKAGE_REQUIRES_PRE "initscripts")
set (CPACK_RPM_PACKAGE_REQUIRES "libyaml,openssl")
set (CPACK_RPM_PACKAGE_LICENSE "GNU General Public License v3.0")
set (CPACK_RPM_PACKAGE_GROUP "Networking Tools")


include (CPack)

