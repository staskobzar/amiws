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

# tests
project (tests C)

# copy fixtures directory for configs testing
file (COPY ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
  DESTINATION ${CMAKE_BINARY_DIR}/test)

add_cmocka_test (test_config test_config.c
                 amiws_lib amiwscfg cmocka)

add_cmocka_test (test_pack_scan test_pack_scan.c
                 amiws_lib amipack mongoose frozen cmocka)
#add_test (test_pack_scan_valgrind valgrind
#  --error-exitcode=1 --read-var-info=yes
#  --leak-check=full --show-leak-kinds=all
#  ./test_pack_scan
#  )

add_cmocka_test (test_create_packet test_create_packet.c
                 amipack cmocka)
#add_test (test_create_packet_valgrind valgrind
#  --error-exitcode=1 --read-var-info=yes
#  --leak-check=full --show-leak-kinds=all
#  ./test_create_packet
#  )

add_cmocka_test (test_pack_type_detect test_pack_type_detect.c
                 amiws_lib amipack mongoose frozen cmocka)
#add_test (test_pack_type_detect_valgrind valgrind
#  --error-exitcode=1 --read-var-info=yes
#  --leak-check=full --show-leak-kinds=all
#  ./test_pack_type_detect
#  )

add_cmocka_test (test_parser_prompt test_parser_prompt.c
                 amipack cmocka)
#add_test (test_parser_prompt_valgrind valgrind
#  --error-exitcode=1 --read-var-info=yes
#  --leak-check=full --show-leak-kinds=all
#  ./test_parser_prompt
#  )

add_cmocka_test (test_parser_message test_parser_message.c
                 amipack cmocka)

add_cmocka_test (test_parser_command test_parser_command.c
                 amipack cmocka)

add_cmocka_test (test_parser_queue test_parser_queue.c
                 amiws_lib amipack mongoose frozen cmocka)

