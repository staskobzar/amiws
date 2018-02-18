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

include (CheckLibraryExists)
check_library_exists (yaml yaml_parser_initialize "" HAVE_YAML)

if (NOT HAVE_YAML)
  message (FATAL_ERROR "YAML library not found. "
                       "Install libyaml and retry.")
endif()

# generate parser with re2c
macro (re2c_gen SRCNAME)
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${SRCNAME}.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${SRCNAME}.re
    COMMAND ${RE2C} --no-generation-date ${ARGV1} ${ARGV2}
            -o ${CMAKE_CURRENT_SOURCE_DIR}/${SRCNAME}.c
            ${CMAKE_CURRENT_SOURCE_DIR}/${SRCNAME}.re
    )
endmacro (re2c_gen)

# generate AMI parser with re2c
if (USE_RE2C)
  find_program (RE2C re2c)
  if (NOT RE2C)
    message (FATAL_ERROR "Can not find 're2c'. Install it to generate AMI parser")
  endif()
  message ("Generate AMI parser with re2c: ${RE2C}")
  re2c_gen (amipack_parser_detect)
  re2c_gen (amipack_parser_prompt "-c")
  re2c_gen (amipack_parser_message "--tags")
  re2c_gen (amipack_parser_command "--tags")
  re2c_gen (amipack_parser_queue "-c" "--tags")
endif (USE_RE2C)

# generate config parser with lemon
if (USE_LEMON)
  find_program (LEMON lemon)
  if (NOT LEMON)
    message (FATAL_ERROR "Can not find 'lemon'. Install it to generate config parser")
  endif()
  message ("Generate config parser with lemon: ${LEMON}")
  add_custom_command (
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/config_parser.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config_parser.y
    COMMAND ${LEMON} ${CMAKE_CURRENT_SOURCE_DIR}/config_parser.y
    )
endif (USE_LEMON)

add_library (amiwscfg config.c config_parser.c config_parser.h amiws.h)
add_library (amiws_lib amiws_lib.c amiws.h)
add_library (mongoose mongoose.c mongoose.h)
add_library (frozen frozen.c frozen.h)
add_library (amipack  amipack.c amipack.h
                      amipack_parser_detect.c
                      amipack_parser_prompt.c
                      amipack_parser_message.c
                      amipack_parser_command.c
                      amipack_parser_queue.c
                      )
target_link_libraries (amiwscfg yaml)

foreach (header
    amiws config_parser mongoose frozen amipack)
  configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/${header}.h"
    "${CMAKE_CURRENT_BINARY_DIR}/${header}.h"
    COPYONLY
    )
endforeach (header)
