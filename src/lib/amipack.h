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
 * @file amiws.h
 * @brief AMI (Asterisk Management Interface) messages
 * read/create functions interface.
 * AMI packet is implemented as linked list of headers.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#ifndef __AMIP_H
#define __AMIP_H

#include <stdlib.h>
#include <string.h>

/*!
 * Return length of the packet as string representation.
 * All headers length + CRLF stanza (2 bytes)
 */
#define amipack_length(pack) (pack)->length + 2

/*! Set AMI packet type. */
#define amipack_type(pack, ptype) (pack)->type = ptype

/*! Number of headers in packet. */
#define amipack_size(pack) (pack)->size

/*! Append Action header. */
#define amipack_append_action(pack, val, len) amipack_append((pack), strdup("Action"), 6, val, len)

/*! AMI packet types. */
enum pack_type {
  AMI_UNKNOWN, AMI_PROMPT, AMI_ACTION, AMI_EVENT, AMI_RESPONSE, AMI_QUEUES
};

/*!
 * AMI semantic version structure. Used when AMI prompt line parsed.
 */
typedef struct AMIVer_ {
  unsigned short major; /*!< major */
  unsigned short minor; /*!< minor */
  unsigned short patch; /*!< patch */
} AMIVer;

/*!
 * AMI header structure.
 */
typedef struct AMIHeader_ {

  char *name;         /*!< AMI header name as string. */
  size_t name_size;   /*!< Name field size */
  char *value;        /*!< AMI header value as string. */
  size_t value_size;  /*!< Value field size */

  struct AMIHeader_   *next; /*!< Next AMI header pointer. Linked list element. */

} AMIHeader;

/*!
 * AMI queue structure.
 * Action "Queues" returns none standard AMI response.
 */
typedef struct AMIQueue_ {
  char *name;
  int calls;
  int maxlen; /*!<Maximum number of people waiting in the queue (0 for unlimited).*/
  char *strategy;
  int holdtime;
  int talktime;
  int weight;
  int callscompleted;
  int callsabandoned;
  char *sl;   /*!< Service level (% of calls answered within X seconds) */
  int sl_sec;
  int members_size;
  int callers_size;

  // Members
  // Callers
} AMIQueue;

/*!
 * AMI packet structure.
 */
typedef struct AMIPacket_ {

  int             size;   /*!< Number of headers. */

  size_t          length; /*!< Total length of all headers as string. */

  enum pack_type  type;   /*!< AMI packet type: Action, Event etc. */

  AMIHeader       *head;  /*!< Linked list head pointer to AMI header. */
  AMIHeader       *tail;  /*!< Linked list tail pointer to AMI header. */

  AMIQueue        *queue; /*!< Queues response structure. */

} AMIPacket;

/**
 * Initiate AMIPacket and allocate memory.
 * AMI packet is implemented as linked list data structure.
 * @return AMIPacket pointer to the new structure.
 */
AMIPacket *amipack_init();

/**
 * Initiate Queue member
 * @return AMIQueue pointer to the new structure
 */
static AMIQueue *amipack_queue_init();

/**
 * Destroy Queue structrue.
 * @param pack    AMI header to destroy
 */
static void amipack_queue_destroy(AMIQueue *queue);

/**
 * Destroy AMI packet and free memory.
 * @param pack    AMI header to destroy
 */
void amipack_destroy(AMIPacket *pack);

/**
 * Create new AMI header with given parameters.
 * Will allocated memory for AMIHeader and return pointer to it.
 * @param name       AMI header name
 * @param name_size  Name field size
 * @param value      AMI header value
 * @param value_size Value field size
 * @return AMIHeader pointer to the new structure.
 */
AMIHeader *amiheader_create(char *name, size_t name_size, char *value, size_t value_size);

/**
 * Destroy AMI header and free memory.
 * @param hdr   AMI header to destroy
 */
void amiheader_destroy(AMIHeader *hdr);

/**
 * Convert AMIPacket to string. Will allocate
 * memory for the string, set packet as string
 * and return pointer to the char array.
 * @param pack  AMI packet structure pointer
 * @param pstr  AMI packet as string
 * @return pointer to AMI packet as string
 */
size_t amipack_to_str(AMIPacket *pack, char **pstr);

/**
 * Convert AMIHeader pstr string.
 * @param hdr       AMI header structure pointer
 * @param buf       Header as string "Name: value\r\n"
 * @return header characters length
 */
size_t amiheader_to_str(AMIHeader *hdr, char *buf);

/**
 * Append header to AMI packet.
 * Will create new AMI header using given type and value string.
 * New header will be appanded to the head of linked list.
 * @param pack      Pointer to AMI packet structure
 * @param hdr_name  AMI header name as string.
 * @param name_size Header name size
 * @param hdr_value AMI header value as string.
 * @param value_size AMI header value size
 * @return -1 if error or number of headers in packet
 */
int amipack_append(AMIPacket *pack, char *hdr_name, size_t name_size, char *hdr_value, size_t value_size);

/**
 * Append AMI header to packet.
 * @param pack      AMI packet structure pointer
 * @param header    AMI header structure pointer
 * @return -1 if error or number of headers in packet
 */
int amipack_list_append (AMIPacket *pack, AMIHeader *header);

/**
 * Search header by header name. Will return header value
 * length in packet exists otherwise -1. Will return only first found
 * header value.
 * @param pack      AMI packet structure pointer
 * @param name      Header name
 * @param value     String where the found value will be stored.
 * @return -1 or length of the header value
 */
size_t amiheader_find(AMIPacket *pack, const char *name, char **value);

/**
 * Parse AMI protocol prompt string when user logged in.
 * Will set AMIver structure with parsed server AMI version.
 * Prompt header example: Asterisk Call Manager/1.1
 * @param packet    Packet received from server as bytes array.
 * @param version   AMIVer struct will be set when packet parsed
 * @return 1 on success or 0 on fail
 */
int amiparse_prompt (const char *packet, AMIVer *version);


/**
 * Parse AMI packet to AMIPacket structure.
 * @param pack_str  Bytes array received from server.
 * @return AMIPacket pointer or NULL if AMI packet failed to parse.
 */
AMIPacket *amiparse_pack (const char *pack_str);

#endif
