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
 * @file amiws_lib.c
 * @brief AMI/web-socket functions.
 *
 * @author Stas Kobzar <stas.kobzar@modulis.ca>
 */

#include "amiws.h"

static struct mg_mgr mgr;

void amiws_init(struct amiws_params* params)
{
  mg_mgr_init(&mgr, params);
  mg_connect(&mgr, params->address, ami_ev_handler);
  mg_connect(&mgr, "tcp://127.0.0.1:5039", ami_ev_handler);
}

void amiws_destroy()
{
  mg_mgr_free(&mgr);
}

void amiws_loop()
{
  mg_mgr_poll(&mgr, POLL_SLEEP);
}

void ami_ev_handler(struct mg_connection *nc,
                    int ev,
                    void *ev_data)
{
  (void)ev_data;
  struct mbuf *io = &nc->recv_mbuf;
  struct amiws_params *prm = (struct amiws_params*) nc->mgr->user_data;
  AMIVer ver;

  switch(ev) {
    case MG_EV_POLL:
      printf("MG_EV_POLL\n");
      break;
    case MG_EV_CONNECT:
      printf("MG_EV_CONNECT\n");
      break;
    case MG_EV_RECV:
      printf("RECV %p [%lu]: %.*s\n", nc->mgr, io->len, (int)io->len, io->buf);
      mbuf_remove(io, io->len);

      /*
      if (amiparse_prompt (io->buf, &ver) == RV_SUCCESS)
        printf("AMI ver: %d.%d.%d\n", ver.major, ver.minor, ver.patch);
      else
        printf("Invalid prompt.\n");
      */
      break;
    case MG_EV_CLOSE:
      printf("MG_EV_CLOSE reconnect %s...\n", prm->address);
      mg_connect(&mgr, prm->address, ami_ev_handler);
      sleep(1);
      break;
    default:
      printf("Event: %d\n", ev);
      break;
  }
}

