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

#include <foostring/foostring.h>
#include <sakura/backend-subdocument.h>
#include <sakura/exception.h>
#include <sakura/dict.h>

static int poliqarp_subdocument_set_open(struct poliqarp_subdocument_set *set,
   const char *basename, const char *setname, struct poliqarp_error *error)
{
   int rc;
   char *fname = string_aformat(POLIQARP_SUBDOCUMENT_ITEM_FORMAT, basename,
      setname);
   if (fname == NULL) {
      poliqarp_error_from_system(error, _("Unable to open subdocument set %s"),
         setname);
      return -1;
   }
   init_file_map(&set->map);
   rc = create_file_map(&set->map, fname);
   if (rc != 0) {
      poliqarp_error_from_system(error, _("Unable to open subdocument set %s"
         " (%s)"), setname, fname);
   }
   free(fname);
   set->current = 0;
   return rc;
}

static void poliqarp_subdocument_set_close(struct poliqarp_subdocument_set *set)
{
   destroy_file_map(&set->map);
}

int poliqarp_subdocument_next(struct poliqarp_subdocument_set *set,
   struct poliqarp_subdocument *subdocument)
{
   if (set->current >= get_file_size(&set->map) / sizeof(*subdocument))
      return -1;
   *subdocument = ((struct poliqarp_subdocument *)get_file_image(&set->map))[set->current++];
   /* endian-neutralize */
   subdocument->corpus_low = en4(subdocument->corpus_low);
   subdocument->corpus_high = en4(subdocument->corpus_high);
   return 0;
}

void poliqarp_subdocument_search(struct poliqarp_subdocument_set *set,
   uint32_t pos)
{
   struct poliqarp_subdocument *arr = (struct poliqarp_subdocument *)get_file_image(&set->map);
   size_t l = set->current, r = (get_file_size(&set->map) / sizeof(*arr)) - 1, m;
   while (l < r) {
      m = (l + r) / 2;
      if (pos >= en4(arr[m].corpus_low) && pos < en4(arr[m].corpus_high)) {
         set->current = m;
         return;
      } else if (pos < en4(arr[m].corpus_low)) {
         r = m - 1;
      } else {
         l = m + 1;
      }
   }
   if (pos >= en4(arr[r].corpus_high))
      set->current = r + 1;
   else
      set->current = r;
}

int poliqarp_backend_subdocument_open(struct poliqarp_backend_subdocument *this,
   const char *base_name, struct poliqarp_error *error)
{
   size_t i;
   int rc;

   rc = poliqarp_newdict_open(&this->name_dict, base_name, false,
      POLIQARP_SUBDOCUMENT_IMAGE_FORMAT, POLIQARP_SUBDOCUMENT_OFFSET_FORMAT,
      _("Unable to open subdocument name dictionary"), error);
   if (rc != 0)
      return -1;

   this->sets = malloc(sizeof(this->sets[0]) * GET_NUM_ITEMS(&this->name_dict));
   if (this->sets == NULL) {
      poliqarp_error_from_system(error, _("Unable to initialize subdocument backend"));
      goto error;
   }
   for (i = 0; i < GET_NUM_ITEMS(&this->name_dict); i++) {
      const char *setname = (const char *)GET_ITEM(&this->name_dict, i);
      rc = poliqarp_subdocument_set_open(this->sets + i, base_name, setname, error);
      if (rc != 0) {
         size_t j;
         for (j = 0; j < i; j++)
            poliqarp_subdocument_set_close(this->sets + j);
         goto error;
      }
   }
   return 0;
error:
   free(this->sets);
   newdict_close(&this->name_dict);
   return -1;
}

void poliqarp_backend_subdocument_close(struct poliqarp_backend_subdocument *this)
{
   size_t i;
   for (i = 0; i < GET_NUM_ITEMS(&this->name_dict); i++)
      poliqarp_subdocument_set_close(this->sets + i);
   newdict_close(&this->name_dict);
   free(this->sets);
}

struct poliqarp_subdocument_set *poliqarp_backend_subdocument_lookup_set(
   struct poliqarp_backend_subdocument *this, const char *set_name)
{
   size_t i;
   for (i = 0; i < GET_NUM_ITEMS(&this->name_dict); i++)
      if (strcmp(set_name, GET_ITEM(&this->name_dict, i)) == 0)
         return this->sets + i;
   return NULL;
}

#ifdef TEST_SUBDOCUMENT

int main(int argc, char *argv[])
{
   struct poliqarp_subdocument_set set;
   struct poliqarp_subdocument ch;
   if (argc < 3)
      return 1;
   poliqarp_subdocument_set_open(&set, argv[1], argv[2]);
   if (argc >= 4)
      poliqarp_subdocument_search(&set, atoi(argv[3]));
   while (poliqarp_subdocument_next(&set, &ch) == 0) {
      printf("%d, %d\n", ch.corpus_low, ch.corpus_high);
   }
   return 0;
}

#endif
