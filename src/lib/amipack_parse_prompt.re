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
 * @file amipack_parse_prompt.c
 * @brief AMI (Asterisk Management Interface) prompt packet parser.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "amipack.h"

// introducing types:re2c for prompt packet
/*! re2c condition markers */
enum yycond_prompt {
  yycinit,
  yycminor,
  yycpatch,
  yycmajor,
};

int amiparse_prompt (const char *packet, AMIVer *ver)
{
  // init version structure
  ver->major = 0;
  ver->minor = 0;
  ver->patch = 0;

  const char *cur = packet;
  int c = yycinit;

/*!re2c
  re2c:define:YYCTYPE  = "unsigned char";
  re2c:define:YYCURSOR = "cur";
  re2c:define:YYMARKER = "packet";
  re2c:define:YYCONDTYPE = "yycond_prompt";
  re2c:define:YYGETCONDITION = "c";
  re2c:define:YYGETCONDITION:naked = 1;
  re2c:define:YYSETCONDITION = "c = @@;";
  re2c:define:YYSETCONDITION:naked = 1;
  re2c:yyfill:enable = 0;

  CRLF = "\r\n";
  DIGIT = [0-9];

  <*> * { return 0; }
  <init> "Asterisk Call Manager/" :=> major

  <minor,patch> CRLF { goto done; }

  <major> DIGIT { ver->major = ver->major * 10 + (yych - '0'); goto yyc_major; }
  <major> "."   { goto yyc_minor; }

  <minor> DIGIT { ver->minor = ver->minor * 10 + (yych - '0'); goto yyc_minor; }
  <minor> "."   { goto yyc_patch; }

  <patch> DIGIT { ver->patch = ver->patch * 10 + (yych - '0'); goto yyc_patch; }
*/

done:
  return 1;
}

