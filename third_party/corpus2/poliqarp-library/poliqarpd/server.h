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

#ifndef POLIQARPD_SERVER_H
#define POLIQARPD_SERVER_H

#include "sockstream.h"

#include <sakura/common/hash-table.h>

#define POLIQARPD_VERSION PACKAGE_VERSION
#define MAX_CONNECTIONS 128

/* Declared later in this file. */
struct connection_list;

/** 
 * A connection, one of the two most important entities in the server. 
 * It holds all information needed for a connection thread to handle
 * incoming requests. 
 */
typedef struct connection {
   uint32_t ip;                   /**< the client's IP */
   sockstream_t *stream;          /**< I/O socket stream */
   pthread_mutex_t mutex;         /**< mutex protecting writes to this
                                       connection's socket stream */
   struct connection_list *connections;   
                                  /**< the collection of connections */
   struct session_list *sessions; /**< the collection of sessions */
   struct hash_table *corpora;    /**< the collection of predefined corpora */
   pthread_t thread;              /**< identifier of the thread that
                                       handles this connection */
   int session;                   /**< identifier of the session bound to
                                       this connection; -1 if none */
   int id;                        /**< offset of this connection into the
                                       connection list */   
   void *private_data;            /**< libpoliqarp's private data */
} connection_t;

/** 
 * A collection of connections. The name can be a bit confusing: this is 
 * not a linked list, but an array with holes protected by a mutex. 
 */
struct connection_list {
   pthread_mutex_t mutex;  /**< mutex protecting the collection */
   connection_t *array[MAX_CONNECTIONS]; /**< the array proper */
};

/**
 * Initiates the server's shutdown sequence.
 */
void server_shutdown();

/** 
 * The server proper. Accepts connections from clients until request for
 * termination, fires up and handles the threads.
 */
void server_loop();

#endif /* POLIQARPD_SERVER_H */
