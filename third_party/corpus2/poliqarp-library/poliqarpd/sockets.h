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

/*
 * A cross-platform way of including TCP-related functions.
 */

#ifndef POLIQARPD_SOCKETS_H
#define POLIQARPD_SOCKETS_H

#include <poliqarp-config.h>

#ifdef _WIN32
#  include <winsock2.h>
#else
#  include <netdb.h>
#  include <unistd.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  define closesocket(x) close(x)
#endif /* _WIN32 */

/** 
 * Initializes the socket functions. Must be called before any of the functions
 * is used.
 * @return 0 on success, -1 on error. 
 */
int init_sockets();

/** 
 * Deallocates any resources allocated by the socket functions. 
 * @return 0 on success, -1 on error. 
 */
int done_sockets();

/**
 * Call after a failing send() to check if peer has disconnected.
 */
bool peer_disconnected();

#endif /* POLIQARPD_SOCKETS_H */
