/**
 * amiws -- Library with functions for read/create AMI packets
 * Copyright (C) 2017, Stas Kobzar <staskobzar@modulis.ca>
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
 * @file amipack.c
 * @brief AMI (Asterisk Management Interface) messages read/create functions.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "amipack.h"

static const char *pack_type_name[] = {
  "AMI_UNKNOWN",
  "AMI_PROMPT",
  "AMI_ACTION",
  "AMI_EVENT",
  "AMI_RESPONSE",
  "AMI_RESPCMD",
  "AMI_QUEUE"
};


AMIPacket *amipack_init()
{
  AMIPacket *pack = (AMIPacket*) malloc(sizeof(AMIPacket));
  pack->size = 0;
  pack->length = 0;
  pack->sid = 0;
  pack->type = AMI_UNKNOWN;
  pack->head = NULL;
  pack->tail = NULL;

  return pack;
}

AMIQueue *amipack_queue_init()
{
  AMIQueue *queue = (AMIQueue*) malloc(sizeof(AMIQueue));
  assert (queue != NULL && "Failed to allocate memory for queue");
  queue->name = NULL;
  queue->calls = 0;
  queue->maxlen = 0;
  queue->strategy = NULL;
  queue->holdtime = 0;
  queue->talktime = 0;
  queue->weight = 0;
  queue->callscompleted = 0;
  queue->callsabandoned = 0;
  queue->sl = NULL;
  queue->sl_sec = 0;
  queue->members_size = 0;
  queue->callers_size = 0;

  return queue;
}

void amipack_queue_destroy(AMIQueue *queue)
{
    if (queue->name) free(queue->name);
    if (queue->strategy) free(queue->strategy);
    if (queue->sl) free(queue->sl);
    free(queue);
    queue = NULL;
}

void amiheader_destroy (AMIHeader *hdr)
{
  if (hdr) {

    if (hdr->name) free(hdr->name);
    if (hdr->value) free(hdr->value);
    free(hdr);

  }

  hdr = NULL;
}


AMIHeader *amiheader_create(char *name,
                            size_t name_size,
                            char *value,
                            size_t value_size)
{
  AMIHeader *header = (AMIHeader *) malloc (sizeof (AMIHeader));
  assert ( header != NULL && "Failed to allocate memory for header");

  // add header name
  header->name = name;
  header->name_size = name_size;

  // add header value
  header->value = value;
  header->value_size = value_size;

  return header;
}


void amipack_destroy (AMIPacket *pack)
{
  AMIHeader *hdr, *hnext;

  for ( hdr = pack->head; hdr != NULL; hdr = hnext) {

    hnext = hdr->next;
    amiheader_destroy (hdr);

  }

  if (pack != NULL) {

    free(pack);
    pack = NULL;

  }
}


size_t amiheader_to_str(AMIHeader *hdr, char *buf)
{
  size_t len = 0;

  for (int i = 0; i < hdr->name_size; i++, len++) {
    buf[len] = hdr->name[i];
  }
  buf[len++] = ':';
  buf[len++] = ' ';

  for (int i = 0; i < hdr->value_size; i++, len++) {
    buf[len] = hdr->value[i];
  }
  buf[len++] = '\r';
  buf[len++] = '\n';

  return len;
}


size_t amipack_to_str(AMIPacket *pack, char **pstr)
{
  size_t len = 0, size = 0;
  char *buf;
  if (pack->size == 0) {
    return 0;
  }

  buf = (char*) malloc(amipack_length(pack));

  for (AMIHeader *hdr = pack->head; hdr; hdr = hdr->next) {
    len = amiheader_to_str (hdr, buf);
    buf  += len;
    size += len;
  }

  *buf++ = '\r';  size++;
  *buf++ = '\n';  size++;
  // rewind pstr
  buf = buf - size;
  *pstr = buf;

  return size;
}


int amipack_append( AMIPacket *pack,
                    char *hdr_name,
                    size_t name_size,
                    char *hdr_value,
                    size_t value_size)
{
  AMIHeader *header = amiheader_create(hdr_name, name_size, hdr_value, value_size);

  return amipack_list_append (pack, header);
}


int amipack_list_append(AMIPacket *pack,
                        AMIHeader *header)
{
  pack->length += header->name_size + header->value_size + 4; // ": " = 2 char and CRLF = 2 char

  // first header becomes head and tail
  if (pack->size == 0) {
    pack->head = header;
  } else {
    pack->tail->next = header;
  }

  pack->tail = header;
  header->next  = NULL; // append function allways add header to tail of packet.

  pack->size++;

  return pack->size;
}


size_t amiheader_find(AMIPacket *pack,
                      const char *name,
                      char **val)
{
  size_t size = strlen(name);
  if (size == 0) return -1;

  for (AMIHeader *hdr = pack->head; hdr; hdr = hdr->next) {

    if (size == hdr->name_size &&
        strncasecmp(name, hdr->name, hdr->name_size) == 0) {

      *val = hdr->value;
      return hdr->value_size;
    }

  }
  return -1;
}


