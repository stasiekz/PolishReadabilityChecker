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

#include <poliqarp-config.h>

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#include <foostring/foostring.h>

#include "sockets.h"
#include "configuration.h"
#include "log.h"
#include "protocol.h"
#include "sockstream.h"
#include "osdep.h"
#include "server.h"
#include "session.h"
#include "utils.h"

#define BACKLOG_SIZE      5

/* When set to 1, the server is in its shutdown phase and should not accept
 * any incoming connections. */
static int server_end = 0;

/* A mutex protecting the above variable. */
static pthread_mutex_t end_mutex = PTHREAD_MUTEX_INITIALIZER;

void server_shutdown()
{
   lock(&end_mutex);
   server_end = 1;
   unlock(&end_mutex);
}

struct grim_reaper_data {
   struct session_list *list;      /**< the session collection */
   void *private_data;             /**< thread's private data */
};

void connection_list_init(struct connection_list *l)
{
   int i;
   pthread_mutex_init(&l->mutex, NULL);
   for (i = 0; i < MAX_CONNECTIONS; i++)
      l->array[i] = NULL;
}

int connection_list_pick_index(struct connection_list *l)
{
   int i, j;

   pthread_mutex_lock(&l->mutex);
   j = -1;
   for (i = 0; i < MAX_CONNECTIONS; i++) {
      if (l->array[i] == NULL) {
         j = i;
         break;
      }
   }
   return j;
}

void connection_list_insert_at(struct connection_list *l, int offset, 
   connection_t *connection)
{
   if (offset >= 0)
      l->array[offset] = connection;
   pthread_mutex_unlock(&l->mutex);
}

int connection_list_delete_unsafe(struct connection_list *l, int offset)
{
   if (l->array[offset]) {
      free(l->array[offset]);
      l->array[offset] = NULL;
      return 0;
   } else {
      return -1;
   }
}

int connection_list_delete(struct connection_list *l, int offset)
{
   int retval = 0;
   
   lock(&l->mutex);
   pthread_detach(l->array[offset]->thread);	
   retval = connection_list_delete_unsafe(l, offset);
   unlock(&l->mutex);

   return retval;
}

void connection_cleanup(struct connection_list *l)
{
   int i;

   lock(&l->mutex);
   for (i = 0; i < MAX_CONNECTIONS; i++) {
      connection_t *conn = l->array[i];
      if (conn) {
         pthread_cancel(conn->thread);
         pthread_join(conn->thread, NULL);
         connection_list_delete_unsafe(l, i);
      }
   }
   unlock(&l->mutex);
   /* There should no other threads alive at this point: */
   pthread_mutex_destroy(&l->mutex);
}

void connection_remove(void *data)
{
   connection_t *conn = (connection_t *)data;
   sockstream_free(conn->stream);
   pthread_mutex_destroy(&conn->mutex);
}

void grim_reaper_cleanup(void *data)
{
   return;
}

void *grim_reaper(void *data)
{
   struct grim_reaper_data *gdata = (struct grim_reaper_data *)data;
   pthread_cleanup_push(grim_reaper_cleanup, data);
   for (;;) {
      poliqarp_sleep(5);
      pthread_testcancel();
      session_remove_idle(gdata->list);
   }
   pthread_cleanup_pop(1);
   return NULL;
}

void *connection_thread(void *data)
{
   connection_t *conn = (connection_t *)data;
   string_t s;
#if 0   
   uint32_t ip = conn->ip;
#endif
  
   pthread_cleanup_push(connection_remove, data);
#if 0
   log_entry(_("connection with %d.%d.%d.%d established"), 
      ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
#endif
   for (;;) {
      s = sockstream_read_string(conn->stream);
      if (string_len(s) == 0) {
         string_free(s);
         break;
      }
      dispatch(s, conn);
   }
#if 0
   log_entry(_("connection with %d.%d.%d.%d closed"), ip & 0xFF,
      (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
#endif
   if (conn->session != -1)
      session_idle(conn->sessions, conn->session);
   pthread_cleanup_pop(1);
   lock(&end_mutex);
   if (!server_end)
      connection_list_delete(conn->connections, conn->id);
   unlock(&end_mutex);
   return NULL;
}

static void corpora_open_iterator(const char *key, const void *value, void *corpora)
{
   int res;
   struct poliqarp_error open_error = poliqarp_error_none;
   struct poliqarp_corpus *corpus;
   progress_t progress;
   corpus = malloc(sizeof(*corpus));
   if (corpus == NULL)
      system_error(_("malloc() failed"));
   progress_init(&progress);
   res = poliqarp_open_corpus(corpus, value, &progress, &open_error);
   progress_destroy(&progress);
   if (res == 0)
      hash_table_set(corpora, key, corpus);
   else {
      fprintf(stderr, "%s\n", poliqarp_error_message_get(&open_error));
      exit(1);
   }
}

static struct hash_table *corpora_open(const struct hash_table *file_names)
{
   if (file_names != NULL) {
      struct hash_table *corpora;
      corpora = malloc(sizeof(*corpora));
      if (corpora == NULL)
         abort();
      if (create_hash_table(corpora, hash_table_num_items(file_names) * 2, 0, NULL) != 0)
         abort();
      hash_table_iterate(file_names, corpora, corpora_open_iterator);
      return corpora;
   }
   else
      return NULL;
}

static void corpora_close_iterator(void *corpus)
{
   poliqarp_close_corpus(corpus);
   free(corpus);
}

static void corpora_close(struct hash_table *corpora)
{
   if (corpora == NULL)
      return;
   destroy_hash_table(corpora, corpora_close_iterator);
   free(corpora);
}

void server_loop()
{
   int sock;
   struct sockaddr_in server_name;
   struct hostent *host;
   struct connection_list connections;
   struct hash_table *corpora;
   struct session_list sessions;
   struct grim_reaper_data reaper_data;
   pthread_t reaper;
   
   init_sockets();
   if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
      socket_error(_("socket() failed"));

/* This compiles on Windows, but has the side-effect that the server will
   happily continue running even if another one is listening on the same port...
   Weird. */
#ifndef _WIN32 
   {
      int sockopt = 1;
      if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof(sockopt)) == -1)
      {
         socket_error(_("setsockopt() failed"));
      }
   }
#endif /* _WIN32 */

   {
      int d1, d2, d3, d4;
      char ch;
      if (sscanf(string_str(cfg.hostname), "%d.%d.%d.%d%c", &d1, &d2, &d3, &d4, &ch) == 4 &&
          0 <= d1 && d1 <= 255 && 0 <= d2 && d2 <= 255 &&
          0 <= d3 && d3 <= 255 && 0 <= d4 && d4 <= 255) {
         server_name.sin_addr.s_addr = htonl(
            ((uint32_t) d1 << 24) | ((uint32_t) d2 << 16) |
            ((uint32_t) d3 << 8) | (uint32_t) d4);
         server_name.sin_family = AF_INET;
      }
      else
      {
         if ((host = gethostbyname(string_str(cfg.hostname))) == NULL)
            resolve_error(_("gethostbyname() failed"));
         memset(&server_name, 0, sizeof(server_name));
         memcpy(&server_name.sin_addr, host->h_addr, host->h_length);
      }
   }
   server_name.sin_port = htons(cfg.port);

   if (bind(sock, (struct sockaddr *)&server_name, sizeof(server_name)) == -1)
      socket_error(_("bind() failed"));
   if (listen(sock, BACKLOG_SIZE) == -1)
      socket_error(_("listen() failed"));

   corpora = corpora_open(cfg.corpora);

   connection_list_init(&connections);
   session_list_init(&sessions);
   srand(time(NULL));
   
   reaper_data.list = &sessions;
   if (pthread_create(&reaper, NULL, grim_reaper, &reaper_data))
      system_error(_("pthread_create() failed"));

   if (cfg.notify_thread_id) {
      notify_readiness(cfg.notify_thread_id);
   }
   if (cfg.detach) {
      if (console_detach() != 0)
         system_error(_("console_detach() failed"));
   }

   bool server_running = true;
   while (server_running) {
      struct timeval tv;
      struct sockaddr_in client_name;
      connection_t *conn;
      int slave_sock, have_conn;
      socklen_t client_length = sizeof (client_name);
      fd_set rdfs;

      memset(&client_name, 0, client_length);
      FD_ZERO(&rdfs);
      FD_SET(sock, &rdfs);
      tv.tv_sec = 1;
      tv.tv_usec = 0;
      
      have_conn = select(sock + 1, &rdfs, NULL, NULL, &tv);
      if (have_conn == -1)
         socket_error(_("select() failed"));
      if (have_conn) {
         int index;
         slave_sock = accept(sock, (struct sockaddr *)&client_name, 
            &client_length);
         if (slave_sock == -1) {
            if (errno == EAGAIN)
               continue;
            else   
               socket_error(_("accept() failed"));
         }
         index = connection_list_pick_index(&connections);
         if (index == -1) {
            socket_write_cstring(slave_sock, 
               "Too many connections, try again later.\n");
            closesocket(slave_sock);
            pthread_mutex_unlock(&connections.mutex);
            continue;
         }

         conn = malloc(sizeof(connection_t));
         conn->ip = ntohl(client_name.sin_addr.s_addr);
         conn->stream = sockstream_create(slave_sock);
         conn->id = index;
         conn->connections = &connections;
         conn->corpora = corpora;
         conn->sessions = &sessions;
         conn->session = -1;
         pthread_mutex_init(&conn->mutex, NULL);

         if (pthread_create(&conn->thread, NULL, connection_thread, 
            (void *)conn)) 
         {
            system_error(_("pthread_create() failed"));
         }
         connection_list_insert_at(&connections, index, conn);
      }
      lock(&end_mutex);
      server_running = !server_end;
      unlock(&end_mutex);
   }

   pthread_cancel(reaper);
   pthread_join(reaper, NULL);
   connection_cleanup(&connections);
   session_cleanup(&sessions);
   corpora_close(corpora);
}
