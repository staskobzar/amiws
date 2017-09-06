/**
 * amiws -- Library with functions for read/create AMI packets
 * Copyright (C) 2016, Stas Kobzar <staskobzar@modulis.ca>
 *
 * This file is part of amiws.
 *
 * amiws is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * amiws is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with amiws.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file amipack_parser_message.c
 * @brief AMI (Asterisk Management Interface) packet parser.
 * This is standard packets like Action, Event, Response etc.
 * Special packets like Command output and Queues list are parsed
 * with different methods.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "amipack.h"

AMIPacket *amipack_parser_message(const char *packet)
{
  AMIPacket *pack = amipack_init ();
  const char *cursor = packet;
  const char *f1, *f2, *v1, *v2;
  /*!stags:re2c format = 'const char *@@;'; */

loop:
  /*!re2c
    re2c:define:YYCTYPE   = char;
    re2c:define:YYCURSOR  = cursor;
    re2c:define:YYMARKER  = packet;
    re2c:yyfill:enable    = 0;

    CRLF    = "\r\n";
    WSP     = [ \t]*;
    ALPHA   = [a-zA-Z];
    DIGIT   = [0-9];
    COLON   = WSP ":" WSP;

    FIELD   = (ALPHA | DIGIT | [-._])+;
    VALUE   = [^\r]*;

    HEADER  = @f1 FIELD @f2 COLON @v1 VALUE @v2 CRLF;

    *       {
              amipack_destroy (pack);
              pack = NULL;
              goto done;
            }
    CRLF    { goto done; }
    HEADER  {
      int field_len = (int)(f2 - f1);
      int value_len   = (int)(v2 - v1);
      amipack_append(pack, strndup(f1, field_len), field_len,
                           strndup(v1, value_len), value_len);
      goto loop;
    }

  */
done:
  return pack;
}
