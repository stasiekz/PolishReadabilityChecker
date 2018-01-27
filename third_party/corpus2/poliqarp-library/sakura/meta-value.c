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

#include <sakura/meta-value.h>

static size_t meta_key_lookup(const struct poliqarp_corpus *corpus, 
   const char *key)
{
   const struct poliqarp_backend_meta *backend_meta = 
      poliqarp_get_const_backend(corpus, meta);
   size_t num_keys = poliqarp_backend_meta_num_keys(backend_meta);
   size_t i;

   for (i = 0; i < num_keys; ++i)
      if (strcmp(key, poliqarp_backend_meta_key_fetch(backend_meta, i)) == 0)
         break;
   if (i == num_keys) {
      errno = ENOENT;
      return (size_t)-1;
   }

   return i;
}

struct poliqarp_meta_value *poliqarp_meta_value_create_date(
   const struct poliqarp_corpus *corpus, const char *key_text, 
   struct poliqarp_meta_date date, enum poliqarp_meta_match_strategy strategy)
{
   struct poliqarp_meta_value *this;
   size_t key;

   key = meta_key_lookup(corpus, key_text);
   if (key == (size_t)-1)
      return NULL;

   this = malloc(sizeof *this);
   this->domain = POLIQARP_DOMAIN_DATE;
   this->strategy = strategy;
   this->key = key;
   this->value_as.date = date;

   return this;
}

struct poliqarp_meta_value *poliqarp_meta_value_create_undefined(
   const struct poliqarp_corpus *corpus, const char *key_text)
{
   struct poliqarp_meta_value *this;
   size_t key;

   key = meta_key_lookup(corpus, key_text);
   if (key == (size_t)-1)
      return NULL;

   this = malloc(sizeof *this);
   this->domain = POLIQARP_DOMAIN_UNDEFINED;
   this->strategy = POLIQARP_STRATEGY_EQUAL;
   this->key = key;

   return this;
}

struct poliqarp_meta_value *poliqarp_meta_value_create_text(
   const struct poliqarp_corpus *corpus, const char *key_text, 
   const struct poliqarp_regexp *pattern)
{
   struct poliqarp_meta_value *this;
   size_t key;

   key = meta_key_lookup(corpus, key_text);
   if (key == (size_t)-1)
      return NULL;

   this = malloc(sizeof *this);
   this->domain = POLIQARP_DOMAIN_TEXT;
   this->strategy = POLIQARP_STRATEGY_EQUAL;
   this->key = key;
   this->value_as.value_pattern = *pattern;

   return this;
}

void poliqarp_meta_value_destroy(struct poliqarp_meta_value *this)
{
   switch (this->domain) {
      case POLIQARP_DOMAIN_DATE:
         break;
      case POLIQARP_DOMAIN_TEXT:
         poliqarp_regexp_destroy(&this->value_as.value_pattern);
         break;
      case POLIQARP_DOMAIN_UNDEFINED:
         break;
   }
   free(this);
}

bool poliqarp_meta_value_can_be_optimized(const struct poliqarp_meta_value *this,
   const struct poliqarp_meta_value *that, enum poliqarp_logic_operator oper)
{
   /* TODO */
   return false;
}

struct poliqarp_value *poliqarp_meta_value_optimize(
   const struct poliqarp_meta_value *this, const struct poliqarp_meta_value *that,
   enum poliqarp_logic_operator oper)
{
   abort(); /* See the TODO above. */
}

int poliqarp_meta_value_compare(const struct poliqarp_meta_value *this,
   const struct poliqarp_meta_value *that)
{
   return this == that ? 0 : 1;
}

static int compare_date(struct poliqarp_meta_date d1, struct poliqarp_meta_date d2)
{
   return d1.year != d2.year ? d1.year - d2.year : 
          d1.month != d2.month ? d1.month - d2.month : 
          d1.day - d2.day;
}

bool poliqarp_meta_value_eval(const struct poliqarp_meta_value *this,
   const struct poliqarp_corpus *corpus, const struct poliqarp_document *document)
{
   const struct poliqarp_backend_meta *backend_meta =
      poliqarp_get_const_backend(corpus, meta);
   struct poliqarp_binary_metadata meta;
   int i;

   switch (this->domain) {
      case POLIQARP_DOMAIN_DATE:
         for (i = document->meta_high - document->meta_low; i > 0; --i) {
            meta = poliqarp_backend_meta_fetch(backend_meta, document->meta_high - i);
            if (meta.type == POLIQARP_METADATA_DATE && meta.key == this->key)
               switch (this->strategy) {
                  case POLIQARP_STRATEGY_EQUAL:
                     return compare_date(meta.value_as.date,
                        this->value_as.date) == 0;
                  case POLIQARP_STRATEGY_GREATER:
                     return compare_date(meta.value_as.date,
                        this->value_as.date) > 0;
                  case POLIQARP_STRATEGY_GREATER_EQUAL:
                     return compare_date(meta.value_as.date,
                        this->value_as.date) >= 0;
                  case POLIQARP_STRATEGY_SMALLER:
                     return compare_date(meta.value_as.date,
                        this->value_as.date) < 0;
                  case POLIQARP_STRATEGY_SMALLER_EQUAL:
                     return compare_date(meta.value_as.date,
                        this->value_as.date) <= 0;
                  default:
                     abort(); /* Should not happen. */
               }
         }
         return false;

      case POLIQARP_DOMAIN_TEXT:
         for (i = document->meta_high - document->meta_low; i > 0; --i) {
            meta = poliqarp_backend_meta_fetch(backend_meta, document->meta_high - i);
            if ((meta.type == POLIQARP_METADATA_SINGLE || 
                 meta.type == POLIQARP_METADATA_MULTI) && meta.key == this->key) 
            {
               if (poliqarp_regexp_match(&this->value_as.value_pattern,
                  poliqarp_backend_meta_value_fetch(backend_meta, 
                  meta.value_as.text)))
               {
                  return true;
               }
            }
         }
         return false;

      case POLIQARP_DOMAIN_UNDEFINED:
         for (i = document->meta_high - document->meta_low; i > 0; --i) {
            meta = poliqarp_backend_meta_fetch(backend_meta, document->meta_high - i);
            if (meta.type == POLIQARP_METADATA_UNDEFINED && meta.key == this->key)
               return true;
         }
         return false;

      default:
         abort(); /* Should not happen. */
   }                            /* switch(this->domain) */
}

void poliqarp_meta_value_modify(struct poliqarp_meta_value *this,
   enum poliqarp_meta_match_strategy strategy)
{
   this->strategy = strategy;
}

/**
 * Parses the text of the form 'yearSEPmonthSEPday' to produce a
 * metadata date value. SEP can be any non-digit character.
 * If day or month and day are omitted, the result has 0 on the respective
 * field.
 *
 * Returns 0 if the expression was successfully formed, -1 if a parse error
 * occurred.
 */
int poliqarp_meta_date_parse(const char *text, struct poliqarp_meta_date *date)
{
   char *ptr;
   unsigned long val;

   date->year = date->month = date->day = 0;

   if (text[0] == '\0')
      return -1;
   val = strtoul(text, &ptr, 10);
   date->year = val;
   if (date->year != val || val == 0)
      return -1; /* integer overflow or 0 */
   if (ptr[0] == '\0')
      return 0;
   if (ptr[1] == '\0')
      return -1; /* no digits after separator */
   val = strtoul(ptr + 1, &ptr, 10);
   date->month = val;
   if (date->month != val || val == 0 || val > 12)
      return -1; /* integer overflow or invalid month */
   if (ptr[0] == '\0')
      return 0;
   if (ptr[1] == '\0')
      return -1; /* no digits after separator */
   val = strtoul(ptr + 1, &ptr, 10);
   date->day = val;
   if (date->day != val || val == 0 || val > 31)
      return -1; /* integer overflow or invalid day */
   if (ptr[0] == '\0')
      return 0;
   else
      return -1; /* trailing garbage */
}
