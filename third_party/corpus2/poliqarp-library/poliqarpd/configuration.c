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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <foostring/foostring.h>

#include <sakura/common/hash-table.h>

#include "configuration.h"

#define DFL_HOSTNAME "127.0.0.1"

#define MAX_PORT ((1 << 16) - 1)
#define DFL_PORT 4567

#define DFL_LOGFILE "poliqarpd.log"

#define MIN_BUFFER_SIZE 1
#define DFL_BUFFER_SIZE 1000
#define MAX_BUFFER_SIZE 50000

#define MIN_IDLE_TIME 1
#define DFL_IDLE_TIME 1200
#define MAX_IDLE_TIME 86400

#define MIN_MATCH_LENGTH 10
#define DFL_MATCH_LENGTH 1000
#define MAX_MATCH_LENGTH 1000

struct configuration cfg = {
   .hostname = NULL,
   .port = DFL_PORT,
   .logging_on = 0,
   .logfile = NULL,
   .match_buffer_size = DFL_BUFFER_SIZE,
   .max_session_idle = DFL_IDLE_TIME,
   .max_match_length = DFL_MATCH_LENGTH,
   .corpora = NULL,
   .allow_any_corpus = false,
   .gui_mode = false,
   .detach = false,
   .notify_thread_id = 0,
   .log = NULL,
   .logmutex = PTHREAD_MUTEX_INITIALIZER,
};

void init_default_cfg() 
{
   cfg.hostname = string_init(DFL_HOSTNAME);
   cfg.logfile = string_init(DFL_LOGFILE);
}

void done_cfg()
{
   if (cfg.hostname) {
      string_free(cfg.hostname);
      cfg.hostname = NULL;
   }
   if (cfg.logfile) {
      string_free(cfg.logfile);
      cfg.logfile = NULL;
   }
   if (cfg.corpora)
   {
      destroy_hash_table(cfg.corpora, free);
      free(cfg.corpora);
      cfg.corpora = NULL;   
   }
}

struct config_hook {
   char *option;
   int (*proc)(const char *, char **);
};

#define STR_RANGE(x, y) STR_RANGE_(x, y)
#define STR_RANGE_(x, y) "" # x " and " # y

static char err_nan[] = _M("not a number");
static char err_invalid_hostname[] = _M("unspecified host name");
static char err_invalid_port[] = _M("port number out of range");
static char err_invalid_logging[] = _M("must be \"on\" or \"off\"");
static char err_invalid_logfile[] = _M("unspecified log file name");
static char err_invalid_buffer_size[] = _M("must be between " STR_RANGE(MIN_BUFFER_SIZE, MAX_BUFFER_SIZE));
static char err_invalid_idle_time[] = _M("must be between " STR_RANGE(MIN_IDLE_TIME, MAX_IDLE_TIME));
static char err_invalid_key_value[] = _M("invalid key:value pair");
static char err_invalid_match_length[] = _M("must be between " STR_RANGE(MIN_MATCH_LENGTH, MAX_MATCH_LENGTH));

static int hostname_hook(const char *val, char **err)
{
   if (strlen(val) == 0) {
      *err = err_invalid_hostname;
      return -1;
   }
   if (cfg.hostname) {
      string_free(cfg.hostname);
      cfg.hostname = NULL;
   }
   cfg.hostname = string_init(val);
   return 0;
}

static int port_hook(const char *val, char **err)
{
   char *serr;
   long port;

   errno = 0;
   port = strtol(val, &serr, 10);
   if (*val == '\0' || *serr != '\0') {
      *err = err_nan;
      return -1;
   }
   if (port < 0 || port > MAX_PORT) {
      *err = err_invalid_port;
      return -1;
   }
   cfg.port = (uint16_t)port;
   return 0;
}

static int logging_hook(const char *val, char **err)
{
   if (!strcmp(val, "on")) {
      cfg.logging_on = 1;
      return 0;
   }
   if (!strcmp(val, "off")) {
      cfg.logging_on = 0;
      return 0;
   }
   *err = err_invalid_logging;
   return -1;
}

static int logfile_hook(const char *val, char **err)
{
   if (strlen(val) == 0) {
      *err = err_invalid_logfile;
      return -1;
   }
   if (cfg.logfile) {
      string_free(cfg.logfile);
      cfg.logfile = NULL;
   }
   cfg.logfile = string_init(val);
   return 0;
}

static int match_buffer_size_hook(const char *val, char **err)
{
   char *serr;
   unsigned long size;

   size = strtoul(val, &serr, 10);
   if (*val == '\0' || *serr != '\0') {
      *err = err_nan;
      return -1;
   }
   if (size < MIN_BUFFER_SIZE)
   {
      cfg.match_buffer_size = MIN_BUFFER_SIZE;
      *err = err_invalid_buffer_size;
      return -1;
   }
   if (size > MAX_BUFFER_SIZE)
   {
      cfg.match_buffer_size = MAX_BUFFER_SIZE;
      *err = err_invalid_buffer_size;
      return -1;
   }
   cfg.match_buffer_size = size;
   return 0;
}

static int max_match_length_hook(const char *val, char **err)
{
   char *serr;
   unsigned long size;
   size = strtoul(val, &serr, 10);
   if (*val == '\0' || *serr != '\0') {
      *err = err_nan;
      return -1;
   }
   if (size < MIN_MATCH_LENGTH)
   {
      cfg.max_match_length = MIN_MATCH_LENGTH;
      *err = err_invalid_match_length;
      return -1;
   }
   if (size > MAX_MATCH_LENGTH)
   {
      cfg.max_match_length = MAX_MATCH_LENGTH;
      *err = err_invalid_match_length;
      return -1;
   }
   cfg.max_match_length = size;
   return 0;
}

static int max_session_idle_hook(const char *val, char **err)
{
   char *serr;
   unsigned long msi;

   msi = strtoul(val, &serr, 10);
   if (*val == '\0' || *serr != '\0') {
      *err = err_nan;
      return -1;
   }
   if (msi < MIN_IDLE_TIME)
   {
      cfg.max_session_idle = MIN_IDLE_TIME;
      *err = err_invalid_idle_time;
      return -1;
   }
   if (msi > MAX_IDLE_TIME)
   {
      cfg.max_session_idle = MAX_IDLE_TIME;
      *err = err_invalid_idle_time;
      return -1;
   }
   cfg.max_session_idle = msi;
   return 0;
}

static int split(char *buf, int len, char ch, char **start1, char **start2);

static int corpus_hook(const char *val, char **err)
{
   char *corpus_key, *corpus_value, *corpus_item;
   if (strcmp(val, "any") == 0)
   {
      cfg.allow_any_corpus = true;
      return 0;
   }
   corpus_item = strdup(val);
   if (corpus_item == NULL)
      abort();
   if (split(corpus_item, strlen(corpus_item), ':', &corpus_key, &corpus_value) != 0)
   {
      *err = err_invalid_key_value;
      free(corpus_item);
      return -1;
   }
   corpus_value = strdup(corpus_value);
   if (corpus_value == NULL)
      abort();
   if (cfg.corpora == NULL)
   {
      cfg.corpora = malloc(sizeof(*cfg.corpora));
      if (cfg.corpora == NULL)
         abort();
      if (create_hash_table(cfg.corpora, 16, HASHTABLE_DUPLICATE_KEYS, NULL) != 0)
         abort();
   }
   if (hash_table_set(cfg.corpora, corpus_key, corpus_value) != 0)
      abort();
   free(corpus_item);
   return 0;
}

static const struct config_hook hooks[] = {
   { "hostname", hostname_hook },
   { "port", port_hook },
   { "logging", logging_hook },
   { "logfile", logfile_hook },
   { "log-file", logfile_hook },
   { "match-buffer-size", match_buffer_size_hook },
   { "max-match-length", max_match_length_hook },
   { "max-session-idle", max_session_idle_hook },
   { "corpus", corpus_hook },
};

/** Searches for a character in the text buffer and splits the text into
 * two strings (before and after the character), trimming whitespaces from
 * the beginning and end of each of the two strings.
 *
 * @param buf the buffer to search in
 * @param len the length of the buffer
 * @param ch the character to search for
 * @param start1 pointer to a variable that will store the beginning of first 
 *      string
 * @param start2 ditto, second string
 * @return 0 if the split was successful
 * @return -1 if the character was not found
 */
static int split(char *buf, int len, char ch, char **start1, char **start2)
{
   char *end1, *end2, *charpos;
   charpos = strchr(buf, ch);
   if (!charpos)
      return -1;
   *start1 = buf;
   end1 = charpos - 1;
   *start2 = charpos  + 1;
   end2 = buf + len - 1;
   while (end1 > *start1 && ascii_isspace(*end1))
      end1--;
   while (end2 > *start2 && ascii_isspace(*end2))
      end2--;
   while (*start1 < end1 && ascii_isspace(**start1))
      (*start1)++;
   while (*start2 < end2 && ascii_isspace(**start2))
      (*start2)++;
   end1[1] = 0;
   end2[1] = 0;
   return 0;
}

static void scan_hooks(const char *name, const char *value, 
   const char *filename, int lineno)
{
   size_t i;
   bool found = false;
   char *err;

   for (i = 0; i < sizeof(hooks) / sizeof(struct config_hook); i++) {
      if (!strcmp(name, hooks[i].option)) {
         found = true;
         if (hooks[i].proc(value, &err) == -1) {
            fprintf(stderr, "%s(%d): %s: %s\n", filename, lineno, name, _(err));
            fflush(stderr);
         }
      }
   }
   
   if (!found) {
      fprintf(stderr, "%s(%d): %s \"%s\"\n", filename, lineno, 
         _("unknown configuration option"), name);
      fflush(stderr);
   }
}

int read_cfg(const char *filename)
{
   FILE *f;
   string_t str;
   int lineno = 0;

   if ((f = fopen(filename, "rt")) == NULL)
      return -1;
   while ((str = string_fgets(f)) != NULL) {
      int len = string_len(str);
      char *name, *value;
      char *buf = string_free_and_get_buffer(str);

      lineno++;
      if (buf[0] == '#' || split(buf, len, '=', &name, &value) == -1) {
         free(buf);
         continue;
      }
      scan_hooks(name, value, filename, lineno);
      free(buf);
   }
   if (fclose(f) == EOF)
      return -1;
   if (cfg.corpora == NULL)
      cfg.allow_any_corpus = true;
   return 0;
}
