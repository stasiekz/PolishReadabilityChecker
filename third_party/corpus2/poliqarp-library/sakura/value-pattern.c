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

#include <sakura/value-pattern.h>

/* create a new vbase */
struct poliqarp_value *poliqarp_value_pattern_create(const struct newdict *dict,
   const struct poliqarp_regexp *regexp)
{
   struct poliqarp_value *this;
   size_t i;

   /* do the allocation */
   this = malloc(sizeof *this);
   this->num_items = dict->num_items;
   this->num_bytes = BIT_ARRAY_LENGTH_BYTES(this->bits, this->num_items);
   this->num_hits = 0;
   this->bits = malloc(this->num_bytes);

   /* clear the bit field */
   memset(this->bits, 0, this->num_bytes);

   if (dict->index != NULL && regexp->simple) {
      void *hit = hash_table_get(dict->index, regexp->pattern);
      if (hit == NULL) /* no hits */
         return this;
      BIT_ARRAY_SET(this->bits, ((intptr_t)hit) - 1);
      ++this->num_hits;
      return this;
   }    
   for (i = 0; i < this->num_items; ++i)
      if (poliqarp_regexp_match(regexp, GET_ITEM(dict, i))) {
         BIT_ARRAY_SET(this->bits, i);
         ++this->num_hits;
      }
   return this;
}
