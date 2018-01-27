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

#include <windows.h>
#include <stdio.h>

#include "osdep.h"

const char *get_config_dir()
{
   return "";
}

void free_config_dir() {
   /* Nothing to do. */
}

void poliqarp_sleep(int seconds)
{
   Sleep(seconds * 1000);
}

void handle_signals()
{
}

int console_detach()
{
   return !FreeConsole();
}

void notify_readiness(unsigned long thread_id)
{
   PostThreadMessage(thread_id, WM_APP, 0, 0); /* deliberately ignore errors */
}

void herror(const char *s)
{
   print_socket_error(s);
}

void print_socket_error(const char *s)
{
   char *message;
   if (s != NULL)
      fprintf(stderr, "%s: ", s);
   switch (WSAGetLastError())
   {
#define MAP(x, y) case WSA##x: message = y; break;
      case 0:
         message = "Success";
         break;
      MAP(EACCES, "Permission denied")
      MAP(EADDRINUSE, "Address already in use")
      MAP(EADDRNOTAVAIL, "Address not available")
      MAP(EAFNOSUPPORT, "Address family not supported")
      MAP(ECONNABORTED, "Connection aborted")
      MAP(ECONNRESET, "Connection reset by peer")
      MAP(EFAULT, "Bad address")
      MAP(EINPROGRESS, "Operation now in progress")
      MAP(EINTR, "Interrupted system call")
      MAP(EINVAL, "Invalid argument")
      MAP(EISCONN, "Socket is already connected")
      MAP(EMFILE, "Too many open files")
      MAP(EMSGSIZE, "Message too long")
      MAP(ENETDOWN, "Network is down")
      MAP(ENETRESET, "Connection aborted by network")
      MAP(ENOBUFS, "No buffer space is available")
      MAP(ENOTSOCK, "Not a socket")
      MAP(EOPNOTSUPP, "Operation not supported on socket")
      MAP(EPROTONOSUPPORT, "Protocol not supported")
      MAP(EPROTOTYPE, "Protocol wrong type for socket")
      MAP(ESHUTDOWN, "Cannot send after transport endpoint shutdown")
      MAP(ESOCKTNOSUPPORT, "Socket type not supported")
      MAP(EWOULDBLOCK, "Operation would block")
      MAP(HOST_NOT_FOUND, "Host not found")
      MAP(NOTINITIALISED, "Successful WSASTARTUP not yet performed")
      MAP(NO_DATA, "Valid name, no data record of requested type")
      MAP(NO_RECOVERY, "A nonrecoverable error occurred")
      MAP(TRY_AGAIN, "Nonauthoritative host not found")
#undef MAP
      default:
         message = "Unknown error";
   }
   fprintf(stderr, "%s\n", message);
}
