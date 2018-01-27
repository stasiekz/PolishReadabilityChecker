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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sakura/poliqarp.h>

#include "sessopt.h"

#define MAX_CONTEXT_CHARS     500
#define MAX_CONTEXT_SEGMENTS  20
#define MAX_WCONTEXT_CHARS    5000
#define MAX_WCONTEXT_SEGMENTS 200

typedef int (*optproc_t)(struct sessopt *, const char *);

struct option_hook {
   char *name;
   optproc_t proc;
};

static int process_width(const char *value, struct context_width *width)
{
   char *endptr;
   unsigned long res;
   errno = 0;
   res = strtoul(value, &endptr, 10);
   if (value == endptr)
      return -1; /* no digits converted */
   if (errno == ERANGE || res != (size_t)res)
      return -1; /* integer overflow */
   if (*endptr == '\0') {
      width->charspec = false;
      width->width = res;
      return 0;
   }
   if ((*endptr == 'c' || *endptr == 'C') && *(endptr + 1) == '\0') {
      width->charspec = true;
      width->width = res;
      return 0;
   }
   return -1;
}

static int process_flag(char value, int *what, int flag)
{
   if (value == '1') {
      *what |= flag;
      return 0;
   }
   if (value == '0') {
      *what &= (~flag);
      return 0;
   }
   return -1;
}

static int process_lemmata(const char *value, int *what)
{
   if (process_flag(value[0], what, SHOW_LEMMATA_LC) ||
       process_flag(value[1], what, SHOW_LEMMATA_LM) ||
       process_flag(value[2], what, SHOW_LEMMATA_RM) ||
       process_flag(value[3], what, SHOW_LEMMATA_RC) ||
       value[4] != '\0')
      return -1;
   else
      return 0;
}

static int process_tags(const char *value, int *what)
{
   if (process_flag(value[0], what, SHOW_TAGS_LC) ||
       process_flag(value[1], what, SHOW_TAGS_LM) ||
       process_flag(value[2], what, SHOW_TAGS_RM) ||
       process_flag(value[3], what, SHOW_TAGS_RC) ||
       value[4] != '\0')
      return -1;
   else
      return 0;
}

static int process_ids(const char *value, int *what)
{
   if (process_flag(value[0], what, SHOW_IDS_LC) ||
       process_flag(value[1], what, SHOW_IDS_LM) ||
       process_flag(value[2], what, SHOW_IDS_RM) ||
       process_flag(value[3], what, SHOW_IDS_RC) ||
       value[4] != '\0')
      return -1;
   else
      return 0;
}

static inline int check_width(const struct context_width *width)
{
   if (width->charspec && (width->width <= 0 || width->width > 
       MAX_CONTEXT_CHARS))
      return -1;
   if (!width->charspec && (width->width <= 0 || width->width > 
       MAX_CONTEXT_SEGMENTS))
      return -1;
   return 0;
}

static inline int wide_check_width(const struct context_width *width)
{
   if (width->charspec && (width->width <= 0 || width->width > 
       MAX_WCONTEXT_CHARS))
      return -1;
   if (!width->charspec && (width->width <= 0 || width->width > 
       MAX_WCONTEXT_SEGMENTS))
      return -1;
   return 0;
}

static int left_context_hook(struct sessopt *options, const char *value)
{
   struct context_width width;
   if (process_width(value, &width) || check_width(&width)) 
      return PE_INVVAL;
   options->left_context_width = width;
   return 0;
}

static int right_context_hook(struct sessopt *options, const char *value)
{
   struct context_width width;
   if (process_width(value, &width) || check_width(&width)) 
      return PE_INVVAL;
   options->right_context_width = width;
   return 0;
}

static int wide_context_hook(struct sessopt *options, const char *value)
{
   struct context_width width;
   if (process_width(value, &width) || wide_check_width(&width)) 
      return PE_INVVAL;
   options->wide_context_width = width;
   return 0;
}

static int lemmata_hook(struct sessopt *options, const char *value)
{
   int res = options->retrieve;
   if (process_lemmata(value, &res))
      return PE_INVVAL;
   options->retrieve = res;
   return 0;
}

static int tags_hook(struct sessopt *options, const char *value)
{
   int res = options->retrieve;
   if (process_tags(value, &res))
      return PE_INVVAL;
   options->retrieve = res;
   return 0;
}

static int ids_hook(struct sessopt *options, const char *value)
{
   int res = options->retrieve;
   if (process_ids(value, &res))
      return PE_INVVAL;
   options->retrieve = res;
   return 0;
}

static int notification_interval_hook(struct sessopt *options, 
   const char *value)
{
   char *endptr;
   unsigned long res = strtoul(value, &endptr, 10);
   if (*value == '\0' || *endptr != '\0' || res > 1000)
      return PE_INVVAL;
   options->interval = res;
   return 0;
}

static int disamb_hook(struct sessopt *options, const char *value)
{
   bool val = true;
   if (strcmp(value, "0") == 0)
      val = false;
   else if (strcmp(value, "1") != 0)
      return PE_INVVAL;
   options->disamb = val;
   return 0;
}

static int query_flags_hook(struct sessopt *options, const char *value)
{
   int opt = 0;
   if (strlen(value) != 4)
      return PE_INVVAL;
   if (value[0] == '1') opt |= POLIQARP_QFLAG_QUERY_I;
   if (value[1] == '1') opt |= POLIQARP_QFLAG_QUERY_X;
   if (value[2] == '1') opt |= POLIQARP_QFLAG_META_I;
   if (value[3] == '1') opt |= POLIQARP_QFLAG_META_X;
   options->qflags = opt;
   return 0;
}

static int rewrite_hook(struct sessopt *options, const char *value)
{
   if (!*value)
      return PE_INVVAL;
   string_clear(options->rewrite);
   string_append_str(options->rewrite, value);
   return 0;
}

static int random_sample_hook(struct sessopt *options, const char *value)
{
   if (strcmp(value, "0") == 0)
      options->random_sample = false;
   else if (strcmp(value, "1") == 0)
      options->random_sample = true;
   else
      return PE_INVVAL;
   return 0;
}

static const struct option_hook hooks[] = {
   { "left-context-width", left_context_hook },
   { "right-context-width", right_context_hook },
   { "wide-context-width", wide_context_hook },
   { "retrieve-lemmata", lemmata_hook },
   { "retrieve-tags", tags_hook },
   { "retrieve-ids", ids_hook },
   { "notification-interval", notification_interval_hook },
   { "query-flags", query_flags_hook },
   { "disamb", disamb_hook },
   { "rewrite", rewrite_hook },
   { "random-sample", random_sample_hook },
};

static const struct sessopt default_options = {
   { 5, false },   /* left context */
   { 5, false },   /* right context */
   { 50, false },  /* wide context */
   SHOW_LEMMATA_LM | SHOW_LEMMATA_RM | SHOW_TAGS_LM | SHOW_TAGS_RM,
   0,
   true,
   POLIQARP_QFLAG_META_I | POLIQARP_QFLAG_META_X,
   NULL
};

void sessopt_init(struct sessopt *options)
{
   *options = default_options;
   options->rewrite = string_init("default");
}

void sessopt_free(struct sessopt *options)
{
   string_free(options->rewrite);
}

int sessopt_set(struct sessopt *options, const char *name, const char *value)
{
   size_t i;
   for (i = 0; i < sizeof(hooks) / sizeof(hooks[0]); i++) 
      if (!strcmp(hooks[i].name, name))
         return hooks[i].proc(options, value);
   return PE_INVOPT;
}
