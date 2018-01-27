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

#include <errno.h>

#include "sockets.h"

int init_sockets() 
{
#ifdef _WIN32
   static WSADATA wsa_data;

   if (WSAStartup(MAKEWORD(2, 0), &wsa_data) != 0)
      return -1;
#endif /* _WIN32 */

   return 0;
}

int done_sockets() 
{
#ifdef _WIN32
   return WSACleanup();
#else
   return 0;
#endif /* _WIN32 */
}

bool peer_disconnected()
{
#ifdef _WIN32
   int err = WSAGetLastError();
   return err == WSAECONNRESET || err == WSAECONNABORTED;
#else
   return errno == ECONNRESET || errno == EPIPE;
#endif /* _WIN32 */
}