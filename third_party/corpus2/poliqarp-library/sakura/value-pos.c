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

#include <sakura/value-pos.h>

struct poliqarp_value *poliqarp_value_create_pos(const struct poliqarp_corpus *corpus,
   const struct poliqarp_regexp *pattern)
{
   struct poliqarp_value *this;

   const struct poliqarp_backend_tag *backend_tag = &corpus->tag;
   const struct poliqarp_backend_config *backend_config = &corpus->config;
   const struct poliqarp_parsed_tag *parsed_tag;
   const struct entity *entity;

   size_t i;
   size_t num_bytes;
   size_t num_hits_pos;

   unsigned int *bitmap;

   /* create bitmap big enough to hold each part of speech */
   num_bytes = BIT_ARRAY_LENGTH_BYTES(bitmap, backend_config->num_pos);
   bitmap = malloc(num_bytes);
   memset(bitmap, 0, num_bytes);

   /* iterate over all entities and skip non part-of-speech ones
    * match the pattern and assign result to bitmap */
   num_hits_pos = 0;
   for (entity = backend_config->named_items.first_entity; entity != NULL;
      entity = entity->next_entity) 
   {
      if (entity->tag == NULL || 
         *(enum poliqarp_entity_type *) entity->tag != POLIQARP_ENTITY_POS)
      {
         continue;
      }
      if (poliqarp_regexp_match(pattern, entity->name)) {
         BIT_ARRAY_SET(bitmap, 
            ((struct poliqarp_part_of_speech *)entity->data)->id);
         num_hits_pos++;
      }
   }

   /* now allocate value */
   this = malloc(sizeof *this);
   this->num_items = poliqarp_backend_tag_num_items(backend_tag);
   this->num_bytes = BIT_ARRAY_LENGTH_BYTES(this->bits, this->num_items);
   this->num_hits = 0;
   this->bits = malloc(this->num_bytes);
   this->domain = POLIQARP_DOMAIN_TAG;

   /* clear the bit field */
   memset(this->bits, 0, this->num_bytes);

   if (num_hits_pos > 0)
      /* iterate over all tags and find those that match */
      for (i = 0; i < this->num_items; ++i) {
         parsed_tag = poliqarp_backend_parsed_tag_fetch(backend_tag, i);
         if (BIT_ARRAY_GET(bitmap, parsed_tag->pos->id)) {
            BIT_ARRAY_SET(this->bits, i);
            ++this->num_hits;
         }
      }
   free(bitmap);
   return this;
}

struct poliqarp_value **poliqarp_value_create_all_pos(const struct poliqarp_corpus *corpus,
   size_t *n)
{
   struct poliqarp_value **values, *this;

   const struct poliqarp_backend_tag *backend_tag = &corpus->tag;
   const struct poliqarp_backend_config *backend_config = &corpus->config;
   const struct poliqarp_parsed_tag *parsed_tag;
   const struct entity *entity;
   size_t n_tags, i;

   *n = backend_config->num_pos;
   values = malloc(*n * sizeof(*this));

   i = 0;
   n_tags = poliqarp_backend_tag_num_items(backend_tag);
   for (entity = backend_config->named_items.first_entity; entity != NULL;
      entity = entity->next_entity) 
   {
      if (entity->tag == NULL || 
         *(enum poliqarp_entity_type *) entity->tag != POLIQARP_ENTITY_POS)
      {
         continue;
      }
      values[i] = this = malloc(sizeof(*this));
      this->num_items = n_tags;
      this->num_bytes = BIT_ARRAY_LENGTH_BYTES(this->bits, n_tags);
      this->bits = malloc(this->num_bytes);
      this->domain = POLIQARP_DOMAIN_TAG;
      this->num_hits = 0;
      memset(this->bits, 0, this->num_bytes);
      i++;
      this++;
   }
   for (i = 0; i < n_tags; i++) {
      parsed_tag = poliqarp_backend_parsed_tag_fetch(backend_tag, i);
      this = values[parsed_tag->pos->id];
      BIT_ARRAY_SET(this->bits, i);
      values[parsed_tag->pos->id]->num_hits++;
   }
   return values;
}
