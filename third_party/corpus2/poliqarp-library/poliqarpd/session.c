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

#include "poliqarp-config.h"

#include "errors.h"
#include "log.h"
#include "session.h"
#include "sockstream.h"
#include "utils.h"

void session_list_init(struct session_list *list)
{
   int i;

   pthread_mutex_init(&list->mutex, NULL);
   list->count = 0;
   for (i = 0; i < MAX_SESSIONS; i++)
      list->sessions[i] = NULL;
}

int session_insert(struct session_list *list, const char *client_id,
   connection_t *conn)
{
   int i, index = -1;
   session_t *session;

   lock(&list->mutex);
   for (i = 0; i < MAX_SESSIONS; i++)
      if (list->sessions[i] == NULL) {
         index = i;
         break;
      }
   if (index == -1) {
      pthread_mutex_unlock(&list->mutex);
      return PE_LIMITEXC;
   }

   session = malloc(sizeof(session_t));
   
   session->match_buffer = malloc(sizeof(struct poliqarp_match_buffer));
   if (poliqarp_create_match_buffer(session->match_buffer,
      cfg.match_buffer_size) != 0) 
   {
      free(session->match_buffer);
      free(session);
      pthread_mutex_unlock(&list->mutex);
      return PE_NOMEM;
   }
   session->index = index;
   session->client_id = string_init(client_id);
   progress_init(&session->progress);
   sessopt_init(&session->options);
   session->job = NULL;
   pthread_mutex_init(&session->mutex, NULL);
   session->cancel = 0;
   session->cleanup = 0;
   session->qeflags = 0;
   pthread_mutex_init(&session->clnmutex, NULL);
   pthread_cond_init(&session->jobnull, NULL);
   session->corpus = NULL;
   session->query = NULL;
   msgqueue_init(&session->messages);
   time(&session->last_used);
   session->connection = conn;
   session->error = poliqarp_error_none;
   poliqarp_srandom_time(&session->random_state);
#ifdef HAVE_LOCALE_T
   session->locale = 0;
#endif
   
   list->sessions[index] = session;
   list->count++;
   unlock(&list->mutex);
   return index;
}

void session_delete(session_t *session)
{
   session_job_cancel(session);
   /* Cancelling a job might have been not possible: */
   lock(&session->mutex);
   while (session->job != NULL)
      pthread_cond_wait(&session->jobnull, &session->mutex);
   unlock(&session->mutex);
   sessopt_free(&session->options);
   progress_destroy(&session->progress);
   poliqarp_destroy_match_buffer(session->match_buffer);
   free(session->match_buffer);
   if (session->query) {
      poliqarp_destroy_query(session->query);
      free(session->query);
      session->query = NULL;
   }
   if (session->corpus) {
      if (session->closable_corpus) {
         poliqarp_close_corpus(session->corpus);
         free(session->corpus);
      }
      session->corpus = NULL;
   }
   string_free(session->client_id);
#ifdef HAVE_LOCALE_T
   if (session->locale)
      freelocale(session->locale);
#endif
   poliqarp_error_message_set(&session->error, NULL);
   pthread_mutex_destroy(&session->mutex);
   pthread_mutex_destroy(&session->clnmutex);
   pthread_cond_destroy(&session->jobnull);
   free(session);
}

void session_remove(struct session_list *list, int session)
{
   lock(&list->mutex);
   if (list->sessions[session]) {
      session_delete(list->sessions[session]);
      list->sessions[session] = NULL;
      list->count--;
      if (list->count == 0 && cfg.gui_mode)
         server_shutdown();
   }
   unlock(&list->mutex);
}

void session_idle(struct session_list *list, int session) 
{
   time_t now = time(NULL);

   lock(&list->mutex);
   if (list->sessions[session]) {
      lock(&list->sessions[session]->mutex);
      list->sessions[session]->connection = NULL;
      list->sessions[session]->last_used = now;
      unlock(&list->sessions[session]->mutex);
   }
   unlock(&list->mutex);
}

void session_remove_idle(struct session_list *list)
{
   int i;
   time_t now = time(NULL);
   
   lock(&list->mutex);
   for (i = 0; i < MAX_SESSIONS; i++) {
      if (list->sessions[i] && list->sessions[i]->connection == NULL && 
         now - list->sessions[i]->last_used >= cfg.max_session_idle)
      {
         session_delete(list->sessions[i]);
         log_entry(_("grim reaper: removing session %d"), i); 
         list->sessions[i] = NULL;
      }
   }
   unlock(&list->mutex);
}

void session_cleanup(struct session_list *list)
{
   int i;
   
   lock(&list->mutex);
   for (i = 0; i < MAX_SESSIONS; i++) {
      if (list->sessions[i]) {
         session_delete(list->sessions[i]);
         list->sessions[i] = NULL;
      }
   }
   unlock(&list->mutex);
   /* There should no other threads alive at this point: */
   pthread_mutex_destroy(&list->mutex);
}

static void flush_pending_messages(session_t *session)
{
   string_t str;
   
   lock(&session->mutex);
   lock(&session->connection->mutex);
   str = msgqueue_get(&session->messages);
   while (str) {
      socket_write_string(session->connection->stream->fd, str);
      string_free(str);
      str = msgqueue_get(&session->messages);
   }
   unlock(&session->connection->mutex);
   unlock(&session->mutex);
}

int session_rebind(struct session_list *list, int session, string_t uid, 
   connection_t *conn)
{
   int res = 0;
   
   if (session < 0 || session >= MAX_SESSIONS)
      return PE_INVSID;

   lock(&list->mutex);
   if (list->sessions[session] == NULL) 
      res = PE_INVSID;
   else if (list->sessions[session]->connection)
      res = PE_SIDUSED;
   else if (string_cmp(uid, list->sessions[session]->client_id))
      res = PE_INVUID;
   else {
      list->sessions[session]->connection = conn;
      conn->session = session;
#if HAVE_LOCALE_T
   uselocale(list->sessions[session]->locale
      ? list->sessions[session]->locale
      : LC_GLOBAL_LOCALE);
#endif
      flush_pending_messages(list->sessions[session]);
   }
   unlock(&list->mutex);

   return res;
}

void session_send(session_t *session, string_t msg)
{
   lock(&session->mutex);
   if (session->connection) {
      lock(&session->connection->mutex);
      socket_write_string(session->connection->stream->fd, msg);
      unlock(&session->connection->mutex);
      string_free(msg);
   } else {
      msgqueue_append(&session->messages, msg);
   }
   unlock(&session->mutex);
}

void session_csend(session_t *session, const char *msg)
{
   session_send(session, string_init(msg));
}

void session_job_launch(session_t *session, void *(*routine)(void *), 
   void *data)
{
   lock(&session->clnmutex);
   session->cleanup = session->cancel = 0;
   unlock(&session->clnmutex);
   session->job = malloc(sizeof(pthread_t));
   pthread_create(session->job, NULL, routine, data);
}

int session_job_cancel(session_t *session)
{
   int res;
   
   lock(&session->mutex);
   if (session->job == NULL)
      res = PPE_INVJID;
   else {
      pthread_cancel(*(session->job));
      pthread_join(*(session->job), NULL);
      free(session->job);
      session->job = NULL;
      pthread_cond_broadcast(&session->jobnull);
      res = 0;
   }
   unlock(&session->mutex);
   return res;
}

void session_job_cleanup(void *sess)
{
   session_t *session = *((session_t **)sess);
   int cleanup;
   lock(&session->clnmutex);
   cleanup = session->cleanup;
   session->cleanup = 1;
   unlock(&session->clnmutex);
   if (cleanup == 1)
      return;
   lock(&session->mutex);   
   pthread_detach(*(session->job));
   free(session->job);
   session->job = NULL;
   pthread_cond_broadcast(&session->jobnull);
   pthread_detach(pthread_self());
   unlock(&session->mutex);
}
