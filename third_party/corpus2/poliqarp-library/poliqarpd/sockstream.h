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

#ifndef POLIQARPD_SOCKSTREAM_H
#define POLIQARPD_SOCKSTREAM_H

#include <foostring/foostring.h>
#include <string.h>

/** 
 * This structure is a streamed view of a connected TCP socket, from which one 
 * can retrieve strings and to which one can send strings. The strings are not
 * actually sent until a flush function is called.
 */
typedef struct sockstream {
   int fd;       /**< descriptor of the socket */
   char *buf;    /**< the internal input buffer */
   size_t start; /**< start of the next portion that will be read */
   size_t used;  /**< number of used bytes in input buffer */
   char *obuf;   /**< the internal output buffer */
   size_t oused; /**< number of used bytes in the output buffer */
} sockstream_t;

/** 
 * Creates a sockstream from a given socket. 
 */
sockstream_t *sockstream_create(int fd);

/** 
 * Closes the socket connected with the given stream and deallocates all its
 * resources. 
 */
void sockstream_free(sockstream_t *sock);

/** 
 * Reads characters from the socket until encountering a newline. Returns 
 * the string consisting of the characters read, without the terminating 
 * newline character. Can return an empty string in case the underlying 
 * recv() call returns 0. 
 */
string_t sockstream_read_string(sockstream_t *sock);

/** 
 * Writes a string to the given socket. 
 */
void socket_write_string(int sock, const string_t str);

/** 
 * Writes a null-terminated string to the given socket. 
 */
void socket_write_cstring(int sock, const char *str);

/** 
 * Writes a string to the given socket followed by a newline. 
 */
void socket_writeln_string(int sock, const string_t str);

/**
 * Writes a string to the socket stream's output buffer. The string is not 
 * sent to the underlying socket immediately, but stored in the buffer and
 * sent only when explicitly flushed, or when the buffer overflows.
 */
void sockstream_write_cstring(sockstream_t *sock, const char *str);

/**
 * Flushes the socket stream's output buffer.
 */
void sockstream_output_flush(sockstream_t *sock);

#endif /* POLIQARPD_SOCKSTREAM_H */
