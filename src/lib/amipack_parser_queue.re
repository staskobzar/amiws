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
 * @brief AMI (Asterisk Management Interface) Queue action output parser.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "amipack.h"

// types:re2c
enum YYCONDTYPE {
  yycheader,
  yycmembers,
  yyccallers,
};

static int get_num(const char *start, const char *end)
{
  int num = 0;
  if (start >= end) return -1;
  for(const char *p = start; p < end; p++) {
    if ( *p < '0' || *p > '9' ) {
      return -1;
    }
    num = num * 10 + (*p - '0');
  }
  return num;
}

// if string is number then convert to int and return
// if string == unlimited then return 0
static int get_maxlen(const char *start, const char *end)
{
  if(strncmp("unlimited", start, (end - start)) == 0) {
    return 0;
  }
  return get_num (start, end);
}

AMIQueue *amipack_parser_queue (const char *packet)
{
  const char *cursor = packet;
  const char *YYCTXMARKER;
  int c = yycheader;
  // header's stags
  const char *name_s, *name_e, *calls_s, *calls_e, *max_s, *max_e,
             *stgy_s, *stgy_e, *hold_s, *hold_e, *tt_s, *tt_e,
             *w_s, *w_e, *c_s, *c_e, *a_s, *a_e, *sl_s, *sl_e,
             *slt_s, *slt_e;

  /*!stags:re2c format = 'const char *@@; '; */

  AMIQueue *queue = amipack_queue_init();
  /*!re2c
    re2c:yyfill:enable    = 0;
    re2c:define:YYCTYPE   = char;
    re2c:define:YYCURSOR  = cursor;
    re2c:define:YYMARKER  = packet;
    re2c:define:YYGETCONDITION = "c";
    re2c:define:YYGETCONDITION:naked = 1;
    re2c:define:YYSETCONDITION = "c = @@;";
    re2c:define:YYSETCONDITION:naked = 1;

    CRLF    = "\r\n";
    SP      = " ";
    ASCII   = [\x1f-\x7e];
    EXTENDED= [\x80-\xff];
    DIGIT   = [0-9];

    HEADER = @name_s [^ ]+ @name_e " has " @calls_s DIGIT+ @calls_e
             " calls (max " @max_s [^)]+ @max_e ") in '" @stgy_s [^']+ @stgy_e "' strategy ("
             @hold_s DIGIT+ @hold_e "s holdtime, " @tt_s DIGIT+ @tt_e "s talktime),"
             " W:" @w_s DIGIT+ @w_e ", C:" @c_s DIGIT+ @c_e ", A:" @a_s DIGIT+ @a_e ","
             " SL:" @sl_s DIGIT+ "." DIGIT+ "%" @sl_e " within " @slt_s DIGIT+ @slt_e "s";

    <*> * {
      amipack_queue_destroy(queue);
      queue = NULL;
      goto done;
    }

    <header> HEADER CRLF {
      int len = (int)(name_e - name_s);
      queue->name       = strndup(name_s, len);
      queue->calls      = get_num(calls_s, calls_e);
      queue->maxlen     = get_maxlen(max_s, max_e);
      queue->strategy   = strndup(stgy_s, (int)(stgy_e - stgy_s));
      queue->holdtime   = get_num(hold_s, hold_e);
      queue->talktime   = get_num(tt_s, tt_e);
      queue->weight     = get_num(w_s, w_e);
      queue->callscompleted = get_num(c_s, c_e);
      queue->callsabandoned = get_num(a_s, a_e);
      queue->sl         = strndup(sl_s, (int)(sl_e - sl_s));
      queue->sl_sec     = get_num(slt_s, slt_e);
      goto yyc_members;
    }

    <members> SP{3} "No Members" CRLF {
      goto yyc_members;
    }

    <members> SP{3} "Members: " CRLF {
      goto yyc_members;
    }

    <members> SP{6} (ASCII|EXTENDED)+ CRLF {
      queue->members_size++;
      goto yyc_members;
    }

    <members> SP{3} "No Callers" CRLF CRLF {
      goto done;
    }

    <members> SP{3} "Callers: " CRLF {
      goto yyc_callers;
    }

    <callers> SP{6} DIGIT+ ". " (ASCII|EXTENDED)+ CRLF {
      queue->callers_size++;
      goto yyc_callers;
    }

    <callers> CRLF { goto done; }

  */
done:
  return queue;
}

