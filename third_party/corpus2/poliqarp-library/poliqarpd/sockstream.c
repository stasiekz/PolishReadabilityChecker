/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2004-2009 by Instytut Podstaw Informatyki Polskiej
 * Akademii Nauk (IPI PAN; Institute of Computer Science, Polish
 * Academy of Sciences; cf. www.ipipan.waw.pl).  All rights reserved.
 * 
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file gpl.txt included in the packaging
 * of this file.  (See http://www.gnu.org/licenses/translations.html for
 * unofficial translations.)
 * 
 * A commercial license is available from IPI PAN (contact
 * Michal.Ciesiolka@ipipan.waw.pl or ipi@ipipan.waw.pl for more
 * information).  Licensees holding a valid commercial license from IPI
 * PAN may use this file in accordance with that license.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING
 * THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.
 */

#include <string.h>

#include "sockstream.h"
#include "sockets.h"
#include "utils.h"

#define BUF_SIZE            4096
#define OUTPUT_BUF_SIZE     32768

sockstream_t *sockstream_create(int fd)
{
   sockstream_t *res = malloc(sizeof(sockstream_t));
   res->fd = fd;
   res->buf = malloc(BUF_SIZE);
   res->start = res->used = 0;
   res->obuf = malloc(OUTPUT_BUF_SIZE);
   res->oused = 0;
   return res;
}

void sockstream_free(sockstream_t *s)
{
   closesocket(s->fd);
   free(s->obuf);
   free(s->buf);
   free(s);
}

string_t sockstream_read_string(sockstream_t *s)
{
   string_t res = string_create();
   char *eol = memchr(s->buf + s->start, '\n', s->used);
   int goon = 1;
   if (eol != NULL) {
      size_t numbytes = eol - s->buf - s->start + 1;
      string_append_strn(res, s->buf + s->start, numbytes - 1);
      s->start += numbytes;
      s->used -= numbytes;
      return res;
   }
   string_append_strn(res, s->buf + s->start, s->used);
   while (goon) {
      size_t numbytes;
      ssize_t recvd;
      s->start = 0;
      recvd = recv(s->fd, s->buf, BUF_SIZE, 0);
      if (recvd < 0)
      {
         if (!peer_disconnected())
            socket_error(_("recv() failed"));
         else
            recvd = 0;
      }
      s->used = recvd;
      if (s->used == 0)
         break;
      eol = memchr(s->buf, '\n', s->used);
      if (eol != NULL) {
         numbytes = eol - s->buf;
         goon = 0;
      } else {
         numbytes = s->used;
      }
      string_append_strn(res, s->buf, numbytes);
      s->start += (numbytes + 1);
      s->used -= (numbytes + 1);
   }
   return res;      
}

static void socket_write_cstring_len(int fd, const char *str, size_t length)
{
   while (length) {
      ssize_t written = send(fd, str, length, 0);
      if (written < 0)
      {
         if (!peer_disconnected())
            socket_error(_("send() failed"));
         return;
      }
      str += written;
      length -= written;
   }
}

void socket_write_string(int sock, const string_t str)
{
   socket_write_cstring_len(sock, string_str(str), string_len(str));
}

void socket_write_cstring(int sock, const char *str)
{
   socket_write_cstring_len(sock, str, strlen(str));
}

void socket_writeln(int sock) 
{
   char newline = '\n';
   int written = send(sock, &newline, 1, 0);
   if (written != 1 && !peer_disconnected())
      socket_error(_("send() failed"));
}

void socket_writeln_string(int sock, const string_t str)
{
   socket_write_string(sock, str);
   socket_writeln(sock);
}

void sockstream_output_flush(sockstream_t *s)
{
   socket_write_cstring_len(s->fd, s->obuf, s->oused);
   s->oused = 0;
}

void sockstream_write_cstring(sockstream_t *s, const char *str)
{
   size_t len = strlen(str), ofs = 0;
   while (ofs < len) {
      size_t towrite = MIN(OUTPUT_BUF_SIZE - s->oused, len - ofs);
      memcpy(s->obuf + s->oused, str + ofs, towrite);
      s->oused += towrite;
      ofs += towrite;
      if (s->oused == OUTPUT_BUF_SIZE)
         sockstream_output_flush(s);
   }
}
