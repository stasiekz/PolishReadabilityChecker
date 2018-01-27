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

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <locale.h>

#include "errors.h"
#include "log.h"
#include "protocol.h"
#include "sockets.h"
#include "sockstream.h"
#include "utils.h"

typedef void (*handler_t)(char *, connection_t *);

typedef struct {
   char *command;
   handler_t handler;
} hook_t;

/** 
 * Returns the first word in the given C string as a string_t and advances
 * the string to the beginning of the next word. 
 * @param str pointer to the string.
 */
static string_t get_word(char **str)
{
   char *start = *str, *end = start;
   string_t res;
   
   while (*end != '\0' && *end != ' ') end++;
   res = string_init_n(start, (size_t)(end - start));
   while (*end == ' ') end++;
   *str = end;
   return res;
}

/**
 * Sends an error message to the connection's socket.
 * @param conn the connection to send the message to.
 * @param err the error number as specified in the protocol specification.
 */
static void send_error(connection_t *conn, int err)
{
   char *buf = string_aformat("R ERR %d\n", err);
   lock(&conn->mutex);
   socket_write_cstring(conn->stream->fd, buf);
   unlock(&conn->mutex);
   free(buf);
}

/**
 * Sends a success message ("R OK") along with additional information
 * to the connection's socket.
 * @param conn the connection to send the message to.
 * @param fmt format of the additional information in the style accepted
 * by string_aformat. If this is NULL, the message will be just "R OK".
 * Note that the format string need not specify the starting space or
 * trailing newline character.
 */
static void send_ok(connection_t *conn, const char *fmt, ...)
{
   va_list l;

   lock(&conn->mutex);
   if (fmt == NULL) {
      socket_write_cstring(conn->stream->fd, "R OK\n");
   } else {
      string_t buf = string_init("R OK ");
      va_start(l, fmt);
      string_vformat_append(buf, fmt, l);
      va_end(l);
      string_append_char(buf, '\n');
      socket_write_string(conn->stream->fd, buf);
      string_free(buf);
   }
   unlock(&conn->mutex);
}

/**
 * Retrieves the first word from a C string. If there are no more words
 * after this one, sends an error message and frees the newly retrieved
 * word.
 * @param str pointer to a C string to retrieve the word from.
 * @param s pointer to the variable where the retrieved word should be stored.
 * @return 0 if the assertion succeeded.
 * @return -1 if the assertion failed.
 */
static int expect_more(char **str, string_t *s, connection_t *conn)
{
   if (**str == '\0') {
      send_error(conn, PPE_ARGCOUNT);
      return -1;
   }
   *s = get_word(str);
   if (**str == '\0') {
      send_error(conn, PPE_ARGCOUNT);
      string_free(*s);
      return -1;
   }
   return 0;
}

/** 
 * Checks for end of string. If the given pointer doesn't point
 * to string "\0", returns a nonzero value and sends error message
 * to the given connection, else returns 0.
 */
static int expect_end(char *str, connection_t *conn)
{
   if (*str != '\0') {
      send_error(conn, PPE_ARGCOUNT);
      return -1;
   }
   return 0;
}

/**
 * Same as expect_more(), but this time the assertion is that there should
 * be no more words after this one.
 */
static int expect_no_more(char **str, string_t *s, connection_t *conn)
{
   if (**str == '\0') {
      send_error(conn, PPE_ARGCOUNT);
      return -1;
   }
   *s = get_word(str);
   if (**str != '\0') {
      send_error(conn, PPE_ARGCOUNT);
      string_free(*s);
      return -1;
   }
   return 0;
}

/** 
 * Calls expect_more() and tries to convert the retrieved word into a decimal
 * integer. 
 * @return 0 if the word is a valid decimal integer.
 * @return -1 if either the conversion or expect_more() failed.
 */
static int expect_more_int(char **str, unsigned long *val, connection_t *conn, int err)
{
   string_t s;
   char *tmp;
   if (expect_more(str, &s, conn))
      return -1;
   errno = 0;
   *val = strtoul(string_str(s), &tmp, 10);
   if (errno == ERANGE || *tmp != '\0') {
      send_error(conn, err);
      string_free(s);
      return -1;
   }
   string_free(s);
   return 0;
}

/**
 * Same as expect_more_int(), but calls expect_no_more().
 */
static int expect_no_more_int(char **str, unsigned long *val, connection_t *conn, 
   int err)
{
   string_t s;
   char *tmp;
   if (expect_no_more(str, &s, conn))
      return -1;
   errno = 0;
   *val = strtoul(string_str(s), &tmp, 10);
   if (errno == ERANGE || *tmp != '\0') {
      send_error(conn, err);
      string_free(s);
      return -1;
   }
   string_free(s);
   return 0;
}

static void h_unimplemented(char *cmd, connection_t *conn) 
{
   lock(&conn->mutex);
   socket_write_cstring(conn->stream->fd, "R UNIMPLEMENTED\n");
   unlock(&conn->mutex);
}

static void h_make_session(char *cmd, connection_t *conn)
{
   int res;
   string_t s;
   
   if (conn->session >= 0) {
      send_error(conn, PPE_SESSBOUND);
      return;
   }

   if (expect_no_more(&cmd, &s, conn)) return;
   
   res = session_insert(conn->sessions, string_str(s), conn);
   switch (res) {
      case PE_LIMITEXC:
      case PE_NOMEM:
         send_error(conn, PPE_NOMEM);
         break;
      default:
         conn->session = res;
         send_ok(conn, "%d", res);
         log_entry(_("%S created session %d"), s, res);
         break;
   }
   string_free(s);
}

static void h_close_session(char *cmd, connection_t *conn)
{
   if (expect_end(cmd, conn)) return;
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   session_remove(conn->sessions, conn->session);
   conn->session = -1;
   send_ok(conn, NULL);
}

static void h_resume_session(char *cmd, connection_t *conn) 
{
   int res;
   string_t uid;
   unsigned long sid;

   if (conn->session >= 0) {
      send_error(conn, PPE_SESSBOUND);
      return;
   }

   if (expect_more_int(&cmd, &sid, conn, PPE_INVSID)) return;
   if (expect_no_more(&cmd, &uid, conn)) return;

   res = session_rebind(conn->sessions, sid, uid, conn);
   switch (res) {
      case PE_INVSID:
         send_error(conn, PPE_INVSID);
         break;
      case PE_SIDUSED:
         send_error(conn, PPE_SIDUSED);
         break;
      case PE_INVUID:
         send_error(conn, PPE_INVUID);
         break;
      default:
         send_ok(conn, NULL);
   }

   string_free(uid);
}

static void h_suspend_session(char *cmd, connection_t *conn)
{
   if (expect_end(cmd, conn)) return;
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   session_idle(conn->sessions, conn->session);
   conn->session = -1;
   send_ok(conn, NULL);
}

struct opener_thread_data {
   session_t *session;
   string_t filename;
#ifdef HAVE_LOCALE_T
   locale_t locale;
#endif
   void *private_data;
};

static void opener_cleanup(void *data)
{
   struct opener_thread_data *odata = (struct opener_thread_data *)data;
#ifdef HAVE_LOCALE_T
   if (odata->locale)
      freelocale(odata->locale);
#endif
   string_free(odata->filename);
   free(odata);
}

static void *opener_thread(void *data) 
{
   struct opener_thread_data *odata = (struct opener_thread_data *)data;
   struct poliqarp_error open_error = poliqarp_error_none;
   string_t ress;
   struct poliqarp_corpus *corpus = NULL;
   bool closable_corpus;
   int res = 0;
   int cancel_state;

   /* Disable cancellation as early as possible: */
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state); 
#ifdef HAVE_LOCALE_T
   lock(&odata->session->mutex);
   if (odata->session->locale)
      odata->locale = duplocale(odata->session->locale);
   if (odata->locale)
      uselocale(odata->locale);
   unlock(&odata->session->mutex);
#endif
   ress = string_create();
   pthread_cleanup_push(opener_cleanup, data);
   if (odata->session->connection->corpora) {
      closable_corpus = false;
      corpus = hash_table_get(odata->session->connection->corpora, string_str(odata->filename));
      if (corpus == NULL && !cfg.allow_any_corpus) {
         poliqarp_error_message_set(&open_error, _("Unknown corpus"));
         res = 1;
      }
      else
         res = 0;
   }
   if (res == 0 && corpus == NULL) {
      closable_corpus = true;
      corpus = malloc(sizeof(struct poliqarp_corpus));
      if (corpus == NULL) {
         poliqarp_error_from_system(&open_error, NULL);
      } else {
         pthread_setcancelstate(cancel_state, NULL);
         /* The only place an actual job cancellation can occur: */
         res = poliqarp_open_corpus(corpus, string_str(odata->filename),
            &odata->session->progress, &open_error);
         pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      }
   }
   lock(&odata->session->mutex);
   odata->session->closable_corpus = closable_corpus;
   if (res == 0) {
      odata->session->corpus = corpus;
      string_format_append(ress, "M OPENED\n");
   } else {
      if (closable_corpus)
         free(corpus);
      odata->session->corpus = NULL;
      string_format_append(ress, "M OPENFAIL %d\n", PPE_SYSOPEN);
   }
   poliqarp_error_set(&odata->session->error, &open_error);
   unlock(&odata->session->mutex);
   session_job_cleanup(data);
   session_send(odata->session, ress);
   pthread_cleanup_pop(1);
   return NULL;
}

static void launch_opener_thread(session_t *session, string_t filename)
{
   struct opener_thread_data *data = malloc(sizeof(struct opener_thread_data));
   data->session = session;
#ifdef HAVE_LOCALE_T
   data->locale = 0;
#endif
   data->filename = string_init(string_str(filename));
   session_job_launch(session, opener_thread, data);
}

static void h_open_corpus(char *cmd, connection_t *conn)
{   
   string_t corpusname;
   session_t *session;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   corpusname = string_init(cmd);
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus != NULL)
      send_error(conn, PPE_CORPUSALR);
   else {
      launch_opener_thread(session, corpusname);
      send_ok(conn, NULL);
   }
   unlock(&session->mutex);
   string_free(corpusname);
}

static void h_get_corpus_stats(char *cmd, connection_t *conn)
{
   session_t *session;
   struct poliqarp_corpus_info info;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_end(cmd, conn)) return;
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL)
      send_error(conn, PPE_NOCORPUS);
   else if (session->job)
      send_error(conn, PPE_BUSY);
   else {
      poliqarp_get_corpus_info(session->corpus, &info);
      send_ok(conn, "%lu %lu %lu %lu",
         (unsigned long) info.num_segments,
         (unsigned long) info.num_types,
         (unsigned long) info.num_lemmata,
         (unsigned long) info.num_tags
      );
   }
   unlock(&session->mutex);
}

static void h_unknown(connection_t *conn)
{
   lock(&conn->mutex);
   socket_write_cstring(conn->stream->fd, "R UNKNOWN\n");
   unlock(&conn->mutex);
}

static void h_ping(char *cmd, connection_t *conn)
{
   if (expect_end(cmd, conn)) return;
   lock(&conn->mutex);
   socket_write_cstring(conn->stream->fd, "R PONG\n");
   unlock(&conn->mutex);
}

static void h_get_version(char *cmd, connection_t *conn)
{
   char *res;

   if (expect_end(cmd, conn)) return;
   res = string_aformat("R %s (%s %d.%d.%d)\n", POLIQARPD_VERSION,
      poliqarp_library_name, poliqarp_major_version, poliqarp_minor_version, 
      poliqarp_revision_number);
   lock(&conn->mutex);
   socket_write_cstring(conn->stream->fd, res);
   unlock(&conn->mutex);
   free(res);
}

static void h_get_job_status(char *cmd, connection_t *conn)
{
   session_t *session;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_end(cmd, conn)) return;
   
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->job == NULL) {
      send_error(conn, PPE_INVJID);
   } else {
      send_ok(conn, "%d", progress_get(&session->progress));
   }
   unlock(&session->mutex);
}

static void h_cancel_job(char *cmd, connection_t *conn)
{
   session_t *session;
   int res;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_end(cmd, conn)) return;
   
   session = conn->sessions->sessions[conn->session];
   res = session_job_cancel(session);
   if (res == 0)
      send_ok(conn, NULL);
   else
      send_error(conn, res);
}

static void h_close_corpus(char *cmd, connection_t *conn)
{
   session_t *session;
   
   if (expect_end(cmd, conn)) return;
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL)
      send_error(conn, PPE_NOCORPUS);
   else if (session->job)
      send_error(conn, PPE_BUSY);
   else {
      if (session->closable_corpus) {
         poliqarp_close_corpus(session->corpus);
         free(session->corpus);
      }
      session->corpus = NULL;
      send_ok(conn, NULL);
   }
   unlock(&session->mutex);
}

static void h_get_num_results(char *cmd, connection_t *conn)
{
   session_t *session;
   struct poliqarp_match_buffer_info info;

   if (expect_end(cmd, conn)) return;
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   session = conn->sessions->sessions[conn->session];
   poliqarp_get_match_buffer_info(session->match_buffer, &info);
   send_ok(conn, "%lu", info.num_results);
}

static void h_get_buffer_state(char *cmd, connection_t *conn)
{
   session_t *session;
   struct poliqarp_match_buffer_info info;

   if (expect_end(cmd, conn)) return;
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   session = conn->sessions->sessions[conn->session];
   poliqarp_get_match_buffer_info(session->match_buffer, &info);
   send_ok(conn, "%lu %lu",
      (unsigned long) info.capacity,
      (unsigned long) info.used
   );
}

static void h_resize_buffer(char *cmd, connection_t *conn)
{
   session_t *session;
   unsigned long newsize;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   session = conn->sessions->sessions[conn->session];
   if (expect_no_more_int(&cmd, &newsize, conn, PPE_INVRANGE)) 
      return;
   lock(&session->mutex);
   if (session->job) {
      send_error(conn, PPE_BUSY);
   } else if (poliqarp_resize_match_buffer(session->match_buffer, newsize) == -1) {
      send_error(conn, PPE_INVRANGE);
   } else
      send_ok(conn, NULL);
   unlock(&session->mutex);
}

static bool strempty(const char *cmd)
{
   while (*cmd) {
      if (*cmd != ' ' || *cmd != '\t')
         return false;
      cmd++;
   }
   return true;
}
 
static void h_make_query(char *cmd, connection_t *conn)
{
   session_t *session;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->job)
      send_error(conn, PPE_BUSY);
   else if (session->corpus == NULL)
      send_error(conn, PPE_NOCORPUS);
   else if (strempty(cmd))
      send_error(conn, PPE_INVQUERY);
   else {
      if (session->query) {
         poliqarp_destroy_query(session->query);
         free(session->query);
      }
      session->query = malloc(sizeof(struct poliqarp_query));
      if (session->query == NULL) {
         send_error(conn, PPE_NOMEM);
         goto end;
      }
      assert(session->corpus != NULL);
      if (poliqarp_create_query(session->query, cmd, session->corpus,
         session->options.qflags, string_str(session->options.rewrite),
         session->options.random_sample ? &session->random_state : NULL,
         &session->error))
      {
         free(session->query);
         session->query = NULL;
         send_error(conn, PPE_INVQUERY);
         log_entry(_("%S/%d makes invalid query: <%s>"), session->client_id, session->index, cmd);
         goto end;
      }
      session->qeflags = session->query->eflags;
      poliqarp_forget(session->match_buffer);
      send_ok(conn, NULL);
      log_entry(_("%S/%d makes query: <%s>, %lu segments to inspect"),
         session->client_id, session->index, cmd, (unsigned long) session->query->area.num_segments);
   }
end: ;
   unlock(&session->mutex);
}

struct query_thread_data {
   session_t *session;
   size_t num;
#ifdef HAVE_LOCALE_T
   locale_t locale;
#endif
   void *private_data;
};

static void query_cleanup(void *data)
{
   struct query_thread_data *qdata = (struct query_thread_data *)data;
#ifdef HAVE_LOCALE_T
   if (qdata->locale)
      freelocale(qdata->locale);
#endif
   free(qdata);
}

static void *query_thread(void *data) 
{
   struct query_thread_data *qdata = (struct query_thread_data *)data;
   int res;
   int cancel_state;

   /* Disable cancellation as early as possible: */
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state); 
   pthread_cleanup_push(query_cleanup, data);
#ifdef HAVE_LOCALE_T
   lock(&qdata->session->mutex);
   if (qdata->session->locale)
      qdata->locale = duplocale(qdata->session->locale);
   if (qdata->locale)
      uselocale(qdata->locale);
   unlock(&qdata->session->mutex);
#endif
   {
      pthread_setcancelstate(cancel_state, NULL);
      /* The only place an actual job cancellation can occur: */
      res = poliqarp_produce(qdata->session->match_buffer, qdata->num,
         qdata->session->query, &qdata->session->progress, qdata->session, 
         qdata->session->options.interval, cfg.max_match_length);
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
   }
   if (res == 0) {
      string_t ress = string_create();
      struct poliqarp_match_buffer_info info;
      poliqarp_get_match_buffer_info(qdata->session->match_buffer, &info);
      string_format_append(ress, "M QUERY-DONE %lu\n", (unsigned long) info.used);
      log_entry(_("%S/%d query is done"), qdata->session->client_id,
         qdata->session->index);
      session_job_cleanup(data);
      session_send(qdata->session, ress);
   }
   else
      session_job_cleanup(data);
   pthread_cleanup_pop(1);
   return NULL;
}

static void launch_query_thread(session_t *session, size_t num)
{
   struct query_thread_data *data = malloc(sizeof(struct query_thread_data));
   data->session = session;
   data->num = num;
#ifdef HAVE_LOCALE_T
   data->locale = 0;
#endif
   session_job_launch(session, query_thread, data);
}

static void h_run_query(char *cmd, connection_t *conn)
{
   session_t *session;
   unsigned long num;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_no_more_int(&cmd, &num, conn, PPE_INVRANGE)) return;

   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex); 
   if (session->corpus == NULL)
      send_error(conn, PPE_NOCORPUS);
   else if (session->query == NULL)
      send_error(conn, PPE_INVQUERY);
   else if (session->job)
      send_error(conn, PPE_BUSY);
   else {
      launch_query_thread(session, num);   
      send_ok(conn, NULL);
   }
   unlock(&session->mutex);
}

static int validate_criteria(const char *criteria)
{
   int len = strlen(criteria);
   if (len < 1 || len > 4)
      return -1;
   while (*criteria) {
      if ((*criteria < 'A' || *criteria > 'H') && 
          (*criteria < 'a' || *criteria > 'h'))
         return -1;
      criteria++;
   }
   return 0;
}

struct sort_thread_data {
   session_t *session;
   string_t criteria;
   void *private_data;
};

static int do_sort(struct poliqarp_match_buffer *match_buffer, char how,
   session_t *session)
{
   struct poliqarp_sort_info info;
   info.ascending = (how < 'a');
   info.atergo = (ascii_toupper(how) >= 'E');
   if (info.atergo)
      how -= ('e' - 'a');
   switch (ascii_toupper(how)) {
      case 'A': info.column = POLIQARP_COLUMN_LEFT_CONTEXT; 
                info.context = session->options.left_context_width.width;
                break;
      case 'B': info.column = POLIQARP_COLUMN_LEFT_MATCH; break;
      case 'C': info.column = POLIQARP_COLUMN_RIGHT_MATCH; break;
      case 'D': info.column = POLIQARP_COLUMN_RIGHT_CONTEXT;
                info.context = session->options.right_context_width.width;
                break;
   }
   return poliqarp_sort_match_buffer(match_buffer, &info, &session->progress);
}

static void sort_cleanup(void *data)
{
   struct sort_thread_data *qdata = (struct sort_thread_data *)data;
   free(qdata);
}

static void *sort_thread(void *data) 
{
   struct sort_thread_data *qdata = (struct sort_thread_data *)data;
   int res = 0, i;
   int cancel_state;
   char *crit;

   /* Disable cancellation as early as possible: */
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_state); 
   pthread_cleanup_push(sort_cleanup, data);

#ifdef HAVE_LOCALE_T
   lock(&qdata->session->mutex);
   if (qdata->session->locale)
      uselocale(qdata->session->locale);
   unlock(&qdata->session->mutex);
#endif
   
   crit = string_str(qdata->criteria);   
   
   pthread_setcancelstate(cancel_state, NULL);
   for (i = strlen(crit) - 1; i >= 0; i--) 
      res = do_sort(qdata->session->match_buffer, crit[i], qdata->session);
   pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
   if (res == 0) {
      string_t ress = string_create();
      string_format_append(ress, "M SORTED\n");
      session_job_cleanup(data);
      session_send(qdata->session, ress);
   }
   else
      session_job_cleanup(data);
   pthread_cleanup_pop(1);
   return NULL;
}

static void launch_sort_thread(session_t *session, const char *criteria)
{
   struct sort_thread_data *data = malloc(sizeof(struct query_thread_data));
   data->session = session;
   data->criteria = string_init(criteria);
   session_job_launch(session, sort_thread, data);
}

static void h_sort_results(char *cmd, connection_t *conn)
{
   session_t *session;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (validate_criteria(cmd)) {
      send_error(conn, PPE_INVCRIT);
      return;
   }
   
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL)
      send_error(conn, PPE_NOCORPUS);
   else if (session->query == NULL)
      send_error(conn, PPE_INVQUERY);
   else if (session->job)
      send_error(conn, PPE_BUSY);
   else {
      launch_sort_thread(session, cmd);
      send_ok(conn, NULL);
   }
   unlock(&session->mutex);
}

#define DISPLAY_BASE 0x01
#define DISPLAY_TAG 0x02
#define DISPLAY_IDS 0x04

static void output_interpretation(sockstream_t *stream, 
   struct poliqarp_interpretation *interp, int what)
{
   struct poliqarp_interpretation_info info;
   
   poliqarp_get_interpretation_info(interp, &info);
   if (what & DISPLAY_BASE) {
      sockstream_write_cstring(stream, "R ");
      sockstream_write_cstring(stream, info.base);
      sockstream_write_cstring(stream, "\n");
   }
   if (what & DISPLAY_TAG) {
      sockstream_write_cstring(stream, "R ");
      sockstream_write_cstring(stream, info.tag);
      sockstream_write_cstring(stream, "\n");
   }
}
   
static void output_segment(sockstream_t *stream, bool disamb,
   size_t nth, struct poliqarp_segment *segment, int what, int seq)
{
   char *buf;
   struct poliqarp_segment_info info;
   struct poliqarp_interpretation_set set;
   struct poliqarp_interpretation_set_info sinfo;
   struct poliqarp_interpretation interp;
   size_t i, num;

   poliqarp_get_segment_info(segment, &info);
   if (disamb)
      poliqarp_get_disambiguated_interpretations(segment, &set);
   else
      poliqarp_get_ambiguous_interpretations(segment, &set);
   poliqarp_get_interpretation_set_info(&set, &sinfo);
   if (!seq) {
      sockstream_write_cstring(stream, "R ");
   }
   if (info.space_before)
      sockstream_write_cstring(stream, " ");
   sockstream_write_cstring(stream, info.text);
   if (!seq) {
      sockstream_write_cstring(stream, "\n");
   }
   if (what == 0)
      return;
   num = sinfo.size;
   buf = string_aformat("R %lu\n", (unsigned long) num);
   sockstream_write_cstring(stream, buf);
   free(buf);
   for (i = 0; i < num; i++) {
      poliqarp_get_interpretation(&set, &interp, i);
      output_interpretation(stream, &interp, what);
   }
   if (what & DISPLAY_IDS) {
      buf = string_aformat("R %lu\n", (unsigned long) nth);
      sockstream_write_cstring(stream, buf);
      free(buf);
   }
}

static void output_range(struct poliqarp_corpus *corpus, bool disamb, 
   sockstream_t *stream, size_t start, size_t end, int lemmata, int tags, 
   int ids, int seq)
{
   char *buf;
   size_t i;

   assert(start <= end);
   if (lemmata) 
      lemmata = DISPLAY_BASE;
   if (tags)
      tags = DISPLAY_TAG;
   if (ids)
      ids = DISPLAY_IDS;
   if (seq) {
      lemmata = tags = ids = 0;
      sockstream_write_cstring(stream, "R ");
   } else {
      buf = string_aformat("R %lu\n", (unsigned long) (end - start));
      sockstream_write_cstring(stream, buf);
      free(buf);
   }
   for (i = start; i < end; i++) {
      struct poliqarp_segment segment;
      poliqarp_get_segment(&segment, corpus, i);
      output_segment(stream, disamb, i, &segment, lemmata | tags | ids, seq);
   }
   if (seq) {
      sockstream_write_cstring(stream, "\n");
   }
}

static void output_match(sockstream_t *stream, session_t *session, size_t ofs,
   int seq)
{
   struct poliqarp_match match;
   struct poliqarp_match document_range;
   size_t lcsize, rcsize;
   size_t lc, rc;
   int retrieve = session->options.retrieve;
   struct poliqarp_corpus *corpus = session->corpus;
   bool disamb = session->options.disamb;

   if (seq) {
      lcsize = rcsize = session->options.wide_context_width.width;
   } else {
      lcsize = session->options.left_context_width.width;
      rcsize = session->options.right_context_width.width;
   }
   poliqarp_get_match(session->match_buffer, &match, ofs);
   poliqarp_get_match_for_document(corpus, match.document, &document_range);
   lc = match.start < document_range.start + lcsize
      ? document_range.start
      : match.start - lcsize;
   rc = match.end + rcsize > document_range.end
      ? document_range.end
      : match.end + rcsize;
   output_range(corpus, disamb, stream, lc, match.start,
      retrieve & SHOW_LEMMATA_LC, retrieve & SHOW_TAGS_LC,
      retrieve & SHOW_IDS_LC, seq);
   if (session->qeflags & POLIQARP_QEFLAG_HAS_ALIGN) {
      output_range(corpus, disamb, stream, match.start, match.focus,
         retrieve & SHOW_LEMMATA_LM, retrieve & SHOW_TAGS_LM,
         retrieve & SHOW_IDS_LM, seq);
      output_range(corpus, disamb, stream, match.focus, match.end, 
         retrieve & SHOW_LEMMATA_RM, retrieve & SHOW_TAGS_RM,
         retrieve & SHOW_IDS_RM, seq);
   } else {
      if (seq)
         sockstream_write_cstring(stream, "R \n");
      output_range(corpus, disamb, stream, match.start, match.end,
         retrieve & SHOW_LEMMATA_LM, retrieve & SHOW_TAGS_LM,
         retrieve & SHOW_IDS_LM, seq);
   }
   output_range(corpus, disamb, stream, match.end, rc,
      retrieve & SHOW_LEMMATA_RC, retrieve & SHOW_TAGS_RC,
      retrieve & SHOW_IDS_RC, seq);
}

static void h_get_results(char *cmd, connection_t *conn)
{
   session_t *session;
   unsigned long i, from, to;
   struct poliqarp_match_buffer_info info;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   
   if (expect_more_int(&cmd, &from, conn, PPE_INVRANGE)) return;
   if (expect_no_more_int(&cmd, &to, conn, PPE_INVRANGE)) return;

   session = conn->sessions->sessions[conn->session];
   if (from > to) {
      send_error(conn, PPE_INVRANGE);
      return;
   }
   
   poliqarp_get_match_buffer_info(session->match_buffer, &info);
   if (to >= info.used) {
      send_error(conn, PPE_INVRANGE);
      return;
   }
      
   lock(&conn->mutex);
   sockstream_write_cstring(conn->stream, "R OK\n");
   for (i = from; i <= to; i++) {
      output_match(conn->stream, session, i, 0);
   }
   sockstream_output_flush(conn->stream);
   unlock(&conn->mutex);
}

static void h_get_last_error(char *cmd, connection_t *conn)
{
   session_t *session;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }

   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   lock(&conn->mutex);
   const char *error_message = poliqarp_error_message_get(&session->error);
   if (error_message) {
      sockstream_write_cstring(conn->stream, "R ERROR ");
      sockstream_write_cstring(conn->stream, error_message);
      sockstream_write_cstring(conn->stream, "\n");
      sockstream_output_flush(conn->stream);
   } else {
      sockstream_write_cstring(conn->stream, "R NOERROR\n");
      sockstream_output_flush(conn->stream);
   }
   unlock(&conn->mutex);
   unlock(&session->mutex);
}

static void h_set_locale(char *cmd, connection_t *conn)
{
   string_t locale_name;
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_no_more(&cmd, &locale_name, conn)) return;
#ifdef HAVE_LOCALE_T
   session_t *session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->locale)
      freelocale(session->locale);
   session->locale = newlocale(LC_ALL_MASK, "", NULL);
   if (session->locale)
   {
      locale_t locale = newlocale(LC_MESSAGES_MASK, string_str(locale_name), session->locale);
      if (locale)
         session->locale = locale;
      else
      {
         freelocale(session->locale);
         session->locale = 0;
      }
   }
   uselocale(session->locale ? session->locale : LC_GLOBAL_LOCALE);
   unlock(&session->mutex);
#endif
   send_ok(conn, NULL);
}

static void h_get_context(char *cmd, connection_t *conn)
{
   session_t *session;
   unsigned long ofs;
   struct poliqarp_match_buffer_info info;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_no_more_int(&cmd, &ofs, conn, PPE_INVRANGE)) return;

   session = conn->sessions->sessions[conn->session];
   poliqarp_get_match_buffer_info(session->match_buffer, &info);
   if (ofs >= info.used) {
      send_error(conn, PPE_INVRANGE);
      return;
   }
      
   lock(&conn->mutex);
   sockstream_write_cstring(conn->stream, "R OK\n");
   output_match(conn->stream, session, ofs, 1);
   sockstream_output_flush(conn->stream);
   unlock(&conn->mutex);
}

static void h_get_metadata(char *cmd, connection_t *conn)
{
   session_t *session;
   unsigned long ofs;
   struct poliqarp_match_buffer_info info;
   struct poliqarp_metadata_set set;
   struct poliqarp_metadata meta;
   struct poliqarp_metadata_info minfo;
   struct poliqarp_match match;
   size_t i, count;
   char *tmp = NULL;
   bool error = false;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_no_more_int(&cmd, &ofs, conn, PPE_INVRANGE)) return;

   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL) {
      send_error(conn, PPE_NOCORPUS);
      error = true;
   }
   unlock(&session->mutex);
   if (error)
      return;
   poliqarp_get_match_buffer_info(session->match_buffer, &info);
   if (ofs >= info.used) {
      send_error(conn, PPE_INVRANGE);
      return;
   }
      
   poliqarp_get_match(session->match_buffer, &match, ofs);
   poliqarp_get_metadata_set(session->corpus, match.document, &set);
   count = poliqarp_metadata_count(&set);
   send_ok(conn, "%lu", (unsigned long) count);
   lock(&conn->mutex);
   for (i = 0; i < count; i++) {
      poliqarp_get_metadata(&set, i, &meta);
      poliqarp_get_metadata_info(&meta, &minfo);
      sockstream_write_cstring(conn->stream, "R ");
      sockstream_write_cstring(conn->stream, minfo.key);
      sockstream_write_cstring(conn->stream, "\n");
      switch (minfo.type) {
         case POLIQARP_META_TEXT:
            tmp = string_aformat("R T %s\n", minfo.value.text);
            break;
         case POLIQARP_META_DATE:
            tmp = string_aformat("R D %d %d %d\n", minfo.value.date.year, 
               minfo.value.date.month, minfo.value.date.day);
            break;
         case POLIQARP_META_UNDEFINED:
            tmp = string_aformat("R U\n");
            break;
      }
      sockstream_write_cstring(conn->stream, tmp);
      free(tmp);
   }
   sockstream_output_flush(conn->stream);
   unlock(&conn->mutex);
}

static void h_get_metadata_types(char *cmd, connection_t *conn)
{
   session_t *session;
   struct poliqarp_metadata_types types;
   size_t i;
   bool error = false;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_end(cmd, conn)) return;
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL) {
      send_error(conn, PPE_NOCORPUS);
      error = true;
   }
   unlock(&session->mutex);
   if (error)
      return;
   poliqarp_get_metadata_types(&types, session->corpus);
   send_ok(conn, "%lu", (unsigned long) types.num_types);
   lock(&conn->mutex);
   for (i = 0; i < types.num_types; i++) {
      sockstream_write_cstring(conn->stream, "R ");
      sockstream_write_cstring(conn->stream, types.types[i].type == POLIQARP_META_TEXT ?
         "T " : "D ");
      sockstream_write_cstring(conn->stream, types.types[i].key);
      sockstream_write_cstring(conn->stream, "\n");
   }
   sockstream_output_flush(conn->stream);
   unlock(&conn->mutex);
   poliqarp_free_metadata_types(&types);
}

static void h_set_option(char *cmd, connection_t *conn)
{
   session_t *session;
   string_t optname;
   
   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_more(&cmd, &optname, conn)) return;

   session = conn->sessions->sessions[conn->session];
   switch (sessopt_set(&session->options, string_str(optname), cmd)) {
      case PE_INVOPT: 
         send_error(conn, PPE_INVOPT);
         break;
      case PE_INVVAL: 
         send_error(conn, PPE_INVVAL);
         break;
      default:  /* 0 */
         send_ok(conn, NULL);
         break;
   }
   
   string_free(optname);
}

static void h_create_alias(char *cmd, connection_t *conn)
{
   session_t *session;
   string_t alias;
   bool error = false;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_more(&cmd, &alias, conn)) return;

   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL) {
      send_error(conn, PPE_NOCORPUS);
      error = true;
   }
   unlock(&session->mutex);
   if (error)
      return;
   if (poliqarp_define_alias(session->corpus, string_str(alias), cmd)) {
      string_free(alias);
      send_error(conn, 25);
      return;
   }

   send_ok(conn, NULL);
   string_free(alias);
}

static void h_delete_alias(char *cmd, connection_t *conn)
{
   session_t *session;
   string_t alias;
   bool error = false;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_no_more(&cmd, &alias, conn)) return;

   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL) {
      send_error(conn, PPE_NOCORPUS);
      error = true;
   }
   unlock(&session->mutex);
   if (error)
      return;
   if (poliqarp_delete_alias(session->corpus, string_str(alias))) {
      string_free(alias);
      send_error(conn, 25);
      return;
   }

   send_ok(conn, NULL);
   string_free(alias);
}

static void h_get_aliases(char *cmd, connection_t *conn)
{
   session_t *session;
   struct poliqarp_alias_list aliases;
   size_t i;
   bool error = false;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_end(cmd, conn)) return;
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL) {
      send_error(conn, PPE_NOCORPUS);
      error = true;
   }
   unlock(&session->mutex);
   if (error)
      return;
   poliqarp_get_aliases(session->corpus, &aliases);

   send_ok(conn, "%lu", (unsigned long) aliases.num_aliases);
   lock(&conn->mutex);
   for (i = 0; i < aliases.num_aliases; i++) {
      sockstream_write_cstring(conn->stream, "R ");
      sockstream_write_cstring(conn->stream, aliases.aliases[i].name);
      sockstream_write_cstring(conn->stream, "\n");
      sockstream_write_cstring(conn->stream, "R ");
      sockstream_write_cstring(conn->stream, aliases.aliases[i].value);
      sockstream_write_cstring(conn->stream, "\n");
   }
   sockstream_output_flush(conn->stream);
   unlock(&conn->mutex);
   poliqarp_free_aliases(&aliases);
}

static void h_halt(char *cmd, connection_t *conn)
{
   if (expect_end(cmd, conn)) return;
   if (conn->ip != 0x7F000001LU) {
      send_error(conn, 26); 
      log_entry(_("%d.%d.%d.%d tried to shutdown the server"), 
         (conn->ip >> 24) & 0xFF,
         (conn->ip >> 16) & 0xFF,
         (conn->ip >> 8) & 0xFF,
         conn->ip & 0xFF
      );
      return;
   }
   server_shutdown(); /* bye bye! */
}

/*
 * Format of the reply:
 *  
 * R OK [number-of-categories] [number-of-grammatical-classes]
 * R [description-of-category-1]
 * ...
 * R [description-of-category-N]
 * R [description-of-class-1]
 * R [description-of-class-2]
 * ...
 * R [description-of-class-M]
 * 
 * where [description-of-category] is of the form
 *
 *    category-name attr1 attr2 ... attrN
 *
 * and [description-of-class] is of the form
 * 
 *    class-name category-1 ... category-N
 *
 * where category-X are names of the appropriate categories, in square brackets
 * if the category is optional.
 *
 * This is an optional command: an implementation might return R UNSUPPORTED.
 */
static void h_get_tagset(char *cmd, connection_t *conn)
{
   session_t *session;
   struct poliqarp_tagset_info info;
   size_t i;
   bool error = false;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_end(cmd, conn))
      return;
   session = conn->sessions->sessions[conn->session];
   lock(&session->mutex);
   if (session->corpus == NULL) {
      send_error(conn, PPE_NOCORPUS);
      error = true;
   }
   unlock(&session->mutex);
   if (error)
      return;
   poliqarp_get_tagset_info(session->corpus, &info);
   send_ok(conn, "%lu %lu",
      (unsigned long) info.num_categories,
      (unsigned long) info.num_classes
   );
   lock(&conn->mutex);
   for (i = 0; i < info.num_categories; i++) {
      sockstream_write_cstring(conn->stream, "R ");
      sockstream_write_cstring(conn->stream, info.categories[i]);
      sockstream_write_cstring(conn->stream, "\n");
   }
   for (i = 0; i < info.num_classes; i++) {
      sockstream_write_cstring(conn->stream, "R ");
      sockstream_write_cstring(conn->stream, info.classes[i]);
      sockstream_write_cstring(conn->stream, "\n");
   }
   sockstream_output_flush(conn->stream);
   unlock(&conn->mutex);
   poliqarp_free_tagset_info(&info);
}

static void append_column_types(string_t str, bool match, 
   const session_t *session)
{
   string_append_str(str, "<");
   if (session->options.retrieve & (match ? SHOW_LEMMATA_LM : SHOW_LEMMATA_LC))
      string_append_str(str, "l");
   if (session->options.retrieve & (match ? SHOW_TAGS_LM : SHOW_TAGS_LC))
      string_append_str(str, "t");
   if (session->options.retrieve & (match ? SHOW_IDS_LM : SHOW_IDS_LC))
      string_append_str(str, "i");
   string_append_str(str, ">");
}

static void h_get_column_types(char *cmd, connection_t *conn)
{
   session_t *session;

   if (conn->session < 0) {
      send_error(conn, PPE_NOSESSION);
      return;
   }
   if (expect_end(cmd, conn))
      return;
   session = conn->sessions->sessions[conn->session];
   if (session->query) {
      string_t res = string_create();
      string_append_str(res, "LC");
      append_column_types(res, false, session);
      if (session->qeflags & POLIQARP_QEFLAG_HAS_ALIGN) {
         string_append_str(res, ":LM");
         append_column_types(res, true, session);
         string_append_str(res, ":RM");
         append_column_types(res, true, session);
      } else {
         string_append_str(res, ":M");
         append_column_types(res, true, session);
      }
      string_append_str(res, ":RC");
      append_column_types(res, false, session);
      send_ok(conn, "%s", string_str(res));
      string_free(res);
   } else {
      send_error(conn, 25);
   }
}

static hook_t hooks[] = {
   { "PING", h_ping },
   { "GET-VERSION", h_get_version },
   { "VERSION", h_get_version }, /* deprecated alias for GET-VERSION */
   { "MAKE-SESSION", h_make_session },
   { "RESUME-SESSION", h_resume_session },
   { "RECONNECT", h_resume_session }, /* deprecated alias for RESUME-SESSION */
   { "CLOSE-SESSION", h_close_session },
   { "SUSPEND-SESSION", h_suspend_session },
   { "OPEN-CORPUS", h_open_corpus },
   { "OPEN", h_open_corpus }, /* deprecated alias for OPEN */
   { "CLOSE-CORPUS", h_close_corpus },
   { "CLOSE", h_close_corpus }, /* deprecated alias for CLOSE */
   { "GET-CORPUS-STATS", h_get_corpus_stats },
   { "CORPUS-STATS", h_get_corpus_stats }, /* deprecated alias for GET-CORPUS-STATUS */
   { "GET-JOB-STATUS", h_get_job_status },
   { "STATUS", h_get_job_status }, /* deprecated alias for GET-JOB-STATUS */
   { "CANCEL-JOB", h_cancel_job },
   { "CANCEL", h_cancel_job }, /* deprecated alias for CANCEL-JOB */
   { "MAKE-QUERY", h_make_query },
   { "RUN-QUERY", h_run_query },
   { "GET-RESULTS", h_get_results },
   { "GET-CONTEXT", h_get_context },
   { "SORT-RESULTS", h_sort_results },
   { "SORT", h_sort_results }, /* deprecated alias for SORT */
   { "GET-BUFFER-STATE", h_get_buffer_state },
   { "GET-NUM-RESULTS", h_get_num_results },
   { "BUFFER-STATE", h_get_buffer_state }, /* deprecated alias for GET-BUFFER-STATE */
   { "BUFFER-SHIFT", h_unimplemented }, /* deprecated */
   { "RESIZE-BUFFER", h_resize_buffer },
   { "BUFFER-RESIZE", h_resize_buffer }, /* deprecated alias for RESIZE-BUFFER*/
   { "SET-OPTION", h_set_option },
   { "SET", h_set_option }, /* deprecated alias for SET */
   { "CREATE-ALIAS", h_create_alias },
   { "DELETE-ALIAS", h_delete_alias },
   { "GET-ALIASES", h_get_aliases },
   { "HALT", h_halt },
   { "GET-METADATA", h_get_metadata },
   { "GET-METADATA-TYPES", h_get_metadata_types },
   { "METADATA", h_get_metadata }, /* deprecated alias for GET-METADATA */
   { "METADATA-TYPES", h_get_metadata_types }, /* deprecated alias for GET-METADATA-TYPES */
   { "GET-TAGSET", h_get_tagset },
   { "GET-COLUMN-TYPES", h_get_column_types },
   { "GET-LAST-ERROR", h_get_last_error },
   { "SET-LOCALE", h_set_locale },
   { NULL, NULL }
};

void dispatch(string_t cmd, connection_t *conn)
{
   hook_t *hook = hooks, *found = NULL;
   char *ccmd = string_str(cmd);
   string_t cmdname = get_word(&ccmd);

   while (hook->command) {
      if (!string_ccmp(cmdname, hook->command)) {
         found = hook;
         break;
      }
      hook++;
   }
   string_free(cmdname);
   if (found) {
      found->handler(ccmd, conn);
   } else {
      h_unknown(conn);
      if (conn->session >= 0) {
         const session_t *session = conn->sessions->sessions[conn->session];
         log_entry(_("%S/%d issued an unknown command: %S"),
            session->client_id, session->index, cmd);
      } else {
         log_entry(_("IP %d.%d.%d.%d (with no session) issued an unknown command: %S"),
            (conn->ip >> 24) & 0xFF,
            (conn->ip >> 16) & 0xFF,
            (conn->ip >> 8) & 0xFF,
            conn->ip & 0xFF,
            cmd
         );
      }
   }
   string_free(cmd);
}
