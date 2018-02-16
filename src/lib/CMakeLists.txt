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

add_library (amiwscfg config.c config_parser.c config_parser.h amiws.h)
add_library (amiws amiws_lib.c amiws.h)
add_library (mongoose mongoose.c mongoose.h)
add_library (frozen frozen.c frozen.h)
add_library (amipack  amipack.c amipack.h
                      amipack_parser_detect.c
                      amipack_parser_prompt.c
                      amipack_parser_message.c
                      amipack_parser_command.c
                      amipack_parser_queue.c
                      )
