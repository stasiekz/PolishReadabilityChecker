/*
 * This file is part of the Poliqarp suite.
 * 
 * Copyright (C) 2008-2009 by Instytut Podstaw Informatyki Polskiej
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

#include <sakura/regexp.h>
#include <sakura/query-rewrite.h>
#include <foostring/foostring.h>

struct poliqarp_query_rewrite {
   struct poliqarp_query_rewrite_rule *first;
   struct poliqarp_query_rewrite_rule *last;
};

struct poliqarp_query_rewrite_rule {
   struct poliqarp_regexp pattern;
   char *replacement;
   struct poliqarp_query_rewrite_rule *next;
};

int poliqarp_create_query_rewrite_table(
   struct poliqarp_query_rewrite_table *this)
{
   return create_hash_table(&this->hash_table, 7, HASHTABLE_DUPLICATE_KEYS, NULL);
}

static struct poliqarp_query_rewrite *poliqarp_create_query_rewrite()
{
   struct poliqarp_query_rewrite *this;
   this = malloc(sizeof *this);
   if (this == NULL)
      return NULL;
   this->first = this->last = NULL;
   return this;
}

static struct poliqarp_query_rewrite_rule *poliqarp_create_query_rewrite_rule(
   const char *pattern, const char *replacement)
{
   int rc;
   struct poliqarp_query_rewrite_rule *this;
   assert(pattern != NULL);
   assert(replacement != NULL);
   this = malloc(sizeof *this);
   if (this == NULL)
      return NULL;
   rc = poliqarp_regexp_create(&this->pattern, pattern, 0, POLIQARP_REG_NO_ANCHORS);
   if (rc != 0) {
      free(this);
      errno = EINVAL;
      return NULL;
   }
   this->replacement = strdup(replacement);
   this->next = NULL;
   return this;
}

static struct poliqarp_query_rewrite_rule *poliqarp_destroy_query_rewrite_rule(struct poliqarp_query_rewrite_rule *this)
{
   struct poliqarp_query_rewrite_rule *next;
   assert(this != NULL);
   poliqarp_regexp_destroy(&this->pattern);
   free(this->replacement);
   next = this->next;
   free(this);
   return next;
}

static void poliqarp_destroy_query_rewrite(void *this)
{
   struct poliqarp_query_rewrite_rule *rule;
   struct poliqarp_query_rewrite *rewrite = this; 
   assert(rewrite != NULL);
   rule = rewrite->first;
   while (rule != NULL)
      rule = poliqarp_destroy_query_rewrite_rule(rule);
   free(this);
}

void poliqarp_destroy_query_rewrite_table(
  struct poliqarp_query_rewrite_table *this)
{
   assert(this != NULL);
   destroy_hash_table(&this->hash_table, poliqarp_destroy_query_rewrite);
}

struct poliqarp_query_rewrite *poliqarp_get_query_rewrite(
   struct poliqarp_query_rewrite_table *this, const char *name, bool create)
{
   struct poliqarp_query_rewrite *rewrite;
   assert(this != NULL);
   if (name == NULL)
      return NULL;
   rewrite = hash_table_get(&this->hash_table, name);
   if (rewrite == NULL && create) {
      rewrite = poliqarp_create_query_rewrite();
      hash_table_set(&this->hash_table, name, rewrite);
   }
   return rewrite;
}

int poliqarp_add_query_rewrite_rule(struct poliqarp_query_rewrite *this,
   const char *pattern, const char *replacement)
{
   struct poliqarp_query_rewrite_rule *rule;
   assert(this != NULL);
   rule = poliqarp_create_query_rewrite_rule(pattern, replacement);
   if (rule == NULL)
      return -1;
   if (this->last == NULL) {
      assert(this->first == NULL);
      this->first = rule;
   }
   else
      this->last->next = rule;
   this->last = rule;
   return 0;
}

char *poliqarp_rewrite_query(struct poliqarp_query_rewrite *this,
   const char *query, unsigned int flags, unsigned int xflags)
{
   struct poliqarp_query_rewrite_rule *rule;
   if (this == NULL)
      return NULL;
   for (rule = this->first; rule != NULL; rule = rule->next) {
      regmatch_t submatches[10];
      if (poliqarp_regexp_match_ex(&rule->pattern, query, 10, submatches)) {
         string_t buffer;
         const char *replacement;
         bool dollar = false;
         buffer = string_create();
         for (replacement = rule->replacement; *replacement; replacement++) {
            if (dollar) {
               switch (*replacement) {
                  case '$':
                     string_append_char(buffer, '$');
                     break;
                  case '<': case '>':
                     if ((xflags & POLIQARP_REG_NO_ANCHORS) != 0)
                        string_append_str(buffer, ".*");
                     break;
                  case 'i': case 'I':
                     if ((flags & REG_ICASE) != 0) 
                        string_append_str(buffer, "/i");
                     break;
                  case '1': case '2': case '3': 
                  case '4': case '5': case '6': 
                  case '7': case '8': case '9': 
                  {
                     regmatch_t *submatch = submatches + (*replacement - '0');
                     if (submatch->rm_so != -1) {
                        regoff_t i;
                        for (i = submatch->rm_so; i < submatch->rm_eo; i++) {
                           const char ch = query[i];
                           switch (ch) {
                              case '"':
                              case '\'':
                              case '\\':
                                 string_append_char(buffer, '\\');
                                 /* fall through */
                              default:
                                 string_append_char(buffer, ch);
                           }
                        }
                     }
                     break;
                  }
                  default:
                     string_append_char(buffer, '$');
                     string_append_char(buffer, *replacement);
                     break;
               }
               dollar = false;
            } else {
               if (*replacement == '$')
                  dollar = true;
               else
                  string_append_char(buffer, *replacement);
            }
         }
         return string_free_and_get_buffer(buffer);
      }
   }
   return NULL;
}
