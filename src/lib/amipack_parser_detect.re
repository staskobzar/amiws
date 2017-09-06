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
 * @file amipack_parser_detect.c
 * @brief AMI (Asterisk Management Interface) packet's type detect.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "amipack.h"

enum pack_type amipack_parser_detect (const char *packet)
{
  const char *cursor = packet;

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

    PROMPT      = "Asterisk Call Manager/";
    ACTION      = 'Action';
    EVENT       = 'Event';
    RESPONSE    = 'Response';
    FOLLOWS     = 'Follows';
    FIELD       = (ALPHA | DIGIT | [-._])+;
    RESPVAL     = ('Error'|'Success'|'Failure');
    VALUE       = [^\r]*;

    HEADER      = FIELD COLON VALUE CRLF;

    IS_PROMPT   = PROMPT DIGIT+ "." DIGIT+ ("." DIGIT)? CRLF;
    IS_ACTION   = ACTION COLON VALUE CRLF HEADER* CRLF;
    IS_EVENT    = EVENT COLON VALUE CRLF HEADER* CRLF;
    IS_RESPONSE = RESPONSE COLON RESPVAL CRLF HEADER* CRLF;
    IS_COMMAND  = RESPONSE COLON FOLLOWS CRLF;
    IS_QUEUE    = [^ ]+ " has " DIGIT+ " calls (max " [^)]+ ") in '" [^']+ "' strategy ";

    *           { return AMI_UNKNOWN; }
    IS_PROMPT   { return AMI_PROMPT;  }
    IS_ACTION   { return AMI_ACTION;  }
    IS_EVENT    { return AMI_EVENT;   }
    IS_RESPONSE { return AMI_RESPONSE;}
    IS_COMMAND  { return AMI_RESPCMD; }
    IS_QUEUE    { return AMI_QUEUE;   }
  */

}

