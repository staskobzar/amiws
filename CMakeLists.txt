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
cmake_minimum_required (VERSION 3.5)

project (amiws VERSION 2.2.1 LANGUAGES C)

# project info
set (PACKAGE_URL "https://github.com/staskobzar/amiws")
set (PACKAGE_BUGREPORT "staskobzar@gmail.com")
set (PACKAGE_ISSUES "https://github.com/staskobzar/amiws/issues")

add_subdirectory (src)
